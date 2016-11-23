#pragma once

#include <list>

#include "SnakeInterface.hpp"

namespace Snake
{

class Segments
{
    struct Position
    {
        int x, y;
    };
public:
    Segments(Direction direction);

    void addSegment(int x, int y);
    bool isCollision(int x, int y) const;
    void addHead(int x, int y);
    std::pair<int, int> nextHead() const;
    std::pair<int, int> removeTail();
    void updateDirection(Direction newDirection);
private:
    Direction m_headDirection;
    std::list<Position> m_segments;
};

} // namespace Snake
