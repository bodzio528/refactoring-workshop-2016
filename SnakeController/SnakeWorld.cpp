#include "SnakeWorld.hpp"

#include "IPort.hpp"
#include "EventT.hpp"

#include "SnakeInterface.hpp"
#include "SnakeSegments.hpp"

namespace Snake
{

World::World(IPort& displayPort, IPort& foodPort, Dimension dimension, Position food)
    : m_displayPort(displayPort),
      m_foodPort(foodPort),
      m_foodPosition(food),
      m_dimension(dimension)
{}

boost::optional<Position> World::tryWalk(Position position) const
{
    return boost::make_optional(m_dimension.isInside(position), position);
}

bool World::tryEat(Position position) const
{
    bool eaten = (m_foodPosition == position);
    if (eaten) {
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
    }
    return eaten;
}

void World::updateFoodPosition(Position position, const Segments &segments)
{
    updateFoodPositionWithCleanPolicy(position, segments, std::bind(&World::sendClearOldFood, this));
}

void World::placeFood(Position position, const Segments &segments)
{
    static auto noCleanPolicy = []{};
    updateFoodPositionWithCleanPolicy(position, segments, noCleanPolicy);
}

void World::sendPlaceNewFood(Position position)
{
    m_foodPosition = position;

    DisplayInd placeNewFood;
    placeNewFood.position = position;
    placeNewFood.value = Cell_FOOD;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewFood));
}

void World::sendClearOldFood()
{
    DisplayInd clearOldFood;
    clearOldFood.position = m_foodPosition;
    clearOldFood.value = Cell_FREE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(clearOldFood));
}

void World::updateFoodPositionWithCleanPolicy(Position position, const Segments &segments, std::function<void ()> clearPolicy)
{
    if (segments.isCollision(position) or not tryWalk(position)) {
        m_foodPort.send(std::make_unique<EventT<FoodReq>>());
        return;
    }

    clearPolicy();
    sendPlaceNewFood(position);
}
} // namespace Snake
