#include "SnakeSegments.hpp"

#include <algorithm>

namespace Snake
{

namespace
{
bool isHorizontal(Direction direction)
{
    return Direction_LEFT == direction or Direction_RIGHT == direction;
}

bool isVertical(Direction direction)
{
    return Direction_UP == direction or Direction_DOWN == direction;
}

bool isPositive(Direction direction)
{
    return (isVertical(direction) and Direction_DOWN == direction)
        or (isHorizontal(direction) and Direction_RIGHT == direction);
}

bool perpendicular(Direction dir1, Direction dir2)
{
    return isHorizontal(dir1) == isVertical(dir2);
}
} // namespace

Segments::Segments(Direction direction)
    : m_headDirection(direction)
{}

void Segments::addSegment(int x, int y)
{
    m_segments.emplace_back(Position{x, y});
}

bool Segments::isCollision(int x, int y) const
{
    return m_segments.end() !=  std::find_if(m_segments.cbegin(), m_segments.cend(),
        [x, y](auto const& segment){ return segment.x == x and segment.y == y; });
}

void Segments::addHead(int x, int y)
{
    m_segments.push_front(Position{x, y});
}

std::pair<int, int> Segments::removeTail()
{
    auto tail = m_segments.back();
    m_segments.pop_back();
    return std::make_pair(tail.x, tail.y);
}

std::pair<int, int> Segments::nextHead() const
{
    Position const& currentHead = m_segments.front();

    Position newHead;
    newHead.x = currentHead.x + (isHorizontal(m_headDirection) ? isPositive(m_headDirection) ? 1 : -1 : 0);
    newHead.y = currentHead.y + (isVertical(m_headDirection) ? isPositive(m_headDirection) ? 1 : -1 : 0);

    return std::make_pair(newHead.x, newHead.y);
}

void Segments::updateDirection(Direction newDirection)
{
    if (perpendicular(m_headDirection, newDirection)) {
        m_headDirection = newDirection;
    }
}

} // namespace Snake
