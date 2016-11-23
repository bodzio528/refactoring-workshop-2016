#include "SnakeWorld.hpp"

namespace Snake
{

World::World(std::pair<int, int> dimension, std::pair<int, int> food)
    : m_foodPosition(food),
      m_dimension(dimension)
{}

void World::setFoodPosition(std::pair<int, int> position)
{
    m_foodPosition = position;
}

std::pair<int, int> World::getFoodPosition() const
{
    return m_foodPosition;
}

bool World::contains(int x, int y) const
{
    return x >= 0 and x < m_dimension.first and y >= 0 and y < m_dimension.second;
}

} // namespace Snake
