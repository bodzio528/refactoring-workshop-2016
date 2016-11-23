#pragma once

#include "SnakePosition.hpp"

namespace Snake
{

class IWorld
{
public:
    virtual ~IWorld() = default;

    virtual bool tryWalk(Position position) const = 0;
    virtual bool tryEat(Position position) const = 0;
};

} // namespace Snake
