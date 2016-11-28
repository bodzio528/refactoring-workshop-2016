#pragma once

namespace Snake
{

struct Position
{
    int x;
    int y;

    bool operator==(Position const& rhs) const { return x == rhs.x and y == rhs.y; }
};

} // namespace Snake
