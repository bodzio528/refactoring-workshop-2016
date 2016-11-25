#pragma once

#include <functional>

#include "SnakeWorld.hpp"
#include "SnakeDimension.hpp"

class IPort;

namespace Snake
{

class TorusWorld : public World
{
public:
    TorusWorld(IPort& displayPort, IPort& foodPort, Dimension dimension, Position food)
        : World(displayPort, foodPort, dimension, food)
    {}

    virtual boost::optional<Position> tryWalk(Position position) const override;
};

} // namespace Snake
