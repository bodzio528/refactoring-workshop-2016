#pragma once

#include "SnakePosition.hpp"

namespace Snake
{

struct Dimension
{
    int width;
    int height;

    bool isInside(Position position) const
    {
        return positiveLessThan(position.x, width)
            and positiveLessThan(position.y, height);
    }
private:
    bool positiveLessThan(int value, int max) const
    {
        return 0 <= value and value < max;
    }
};

} // namespace Snake
