#pragma once

#include <boost/optional.hpp>

#include "SnakePosition.hpp"

namespace Snake
{

class IWorld
{
public:
    virtual ~IWorld() = default;

    virtual boost::optional<Position> tryWalk(Position position) const = 0;
    virtual bool tryEat(Position position) const = 0;
};

} // namespace Snake
