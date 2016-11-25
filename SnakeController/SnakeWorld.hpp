#pragma once

#include <functional>

#include "SnakeIWorld.hpp"
#include "SnakeDimension.hpp"

class IPort;

namespace Snake
{
class Segments;

class World : public IWorld
{
public:
    World(IPort& displayPort, IPort& foodPort, Dimension dimension, Position food);
    ~World() override = default;

    boost::optional<Position> tryWalk(Position position) const override;
    bool tryEat(Position position) const override;

    void updateFoodPosition(Position position, Segments const& segments);
    void placeFood(Position position, Segments const& segments);

protected:
    IPort& m_displayPort;
    IPort& m_foodPort;

    Position m_foodPosition;
    Dimension m_dimension;

    void sendPlaceNewFood(Position position);
    void sendClearOldFood();
    void updateFoodPositionWithCleanPolicy(Position position, Segments const& segments, std::function<void()> clearPolicy);
};

} // namespace Snake
