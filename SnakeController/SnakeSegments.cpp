#include "SnakeSegments.hpp"

#include <algorithm>

#include "IPort.hpp"
#include "EventT.hpp"

#include "SnakeIWorld.hpp"

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
    return Direction_DOWN == direction or Direction_RIGHT == direction;
}

bool perpendicular(Direction dir1, Direction dir2)
{
    return isHorizontal(dir1) == isVertical(dir2);
}
} // namespace

Segments::Segments(IPort& displayPort, IPort& scorePort, Direction direction)
    : m_displayPort(displayPort),
      m_scorePort(scorePort),
      m_headDirection(direction)
{}

void Segments::addSegment(Position position)
{
    m_segments.emplace_back(position);
}

bool Segments::isCollision(Position position) const
{
    return m_segments.end() != std::find(m_segments.cbegin(), m_segments.cend(), position);
}

void Segments::addHead(Position position)
{
    m_segments.push_front(position);
}

Position Segments::removeTail()
{
    auto tail = m_segments.back();
    m_segments.pop_back();
    return tail;
}

Position Segments::nextHead() const
{
    auto currentHead = m_segments.front();

    Position newHead;
    newHead.x = currentHead.x + (isHorizontal(m_headDirection) ? isPositive(m_headDirection) ? 1 : -1 : 0);
    newHead.y = currentHead.y + (isVertical(m_headDirection) ? isPositive(m_headDirection) ? 1 : -1 : 0);

    return newHead;
}

void Segments::updateDirection(Direction newDirection)
{
    if (perpendicular(m_headDirection, newDirection)) {
        m_headDirection = newDirection;
    }
}

void Segments::nextStep(IWorld const& world)
{
    updateSegments(nextHead(), world);
}

void Segments::removeTailSegment()
{
    DisplayInd clearTail;
    clearTail.position = removeTail();
    clearTail.value = Cell_FREE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(clearTail));
}

void Segments::addHeadSegment(Position position)
{
    addHead(position);

    DisplayInd placeNewHead;
    placeNewHead.position = position;
    placeNewHead.value = Cell_SNAKE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewHead));
}

void Segments::removeTailSegmentIfNotScored(Position position, IWorld const& world)
{
    if (world.tryEat(position)) {
        ScoreInd scoreInd{};
        scoreInd.score = m_segments.size() - 1;
        m_scorePort.send(std::make_unique<EventT<ScoreInd>>(scoreInd));
    } else {
        removeTailSegment();
    }
}

void Segments::updateSegments(Position position, IWorld const& world)
{
    auto nextPosition = world.tryWalk(position);

    if (not nextPosition.is_initialized() or isCollision(*nextPosition)) {
        m_scorePort.send(std::make_unique<EventT<LooseInd>>());
    } else {
        addHeadSegment(*nextPosition);
        removeTailSegmentIfNotScored(*nextPosition, world);
    }
}

} // namespace Snake
