#pragma once

#include <functional>

#include "SnakePosition.hpp"
#include "SnakeDimension.hpp"

class IPort;

namespace Snake
{
class Segments;

class World
{
public:
    World(IPort& displayPort, IPort& foodPort, Dimension dimension, Position food);


    bool contains(Position position) const;
    bool eatFood(Position position) const;

    void updateFoodPosition(Position position, Segments const& segments);
    void placeFood(Position position, Segments const& segments);

private:
    IPort& m_displayPort;
    IPort& m_foodPort;

    Position m_foodPosition;
    Dimension m_dimension;

    void sendPlaceNewFood(Position position);
    void sendClearOldFood();
    void updateFoodPositionWithCleanPolicy(Position position, Segments const& segments, std::function<void()> clearPolicy);
};

} // namespace Snake
