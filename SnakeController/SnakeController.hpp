#pragma once

#include <list>
#include <memory>

#include "IEventHandler.hpp"
#include "SnakeInterface.hpp"

class Event;
class IPort;

namespace Snake
{
struct ConfigurationError : std::logic_error
{
    ConfigurationError();
};

struct UnexpectedEventException : std::runtime_error
{
    UnexpectedEventException();
};

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

class Controller : public IEventHandler
{
public:
    Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config);

    Controller(Controller const& p_rhs) = delete;
    Controller& operator=(Controller const& p_rhs) = delete;

    void receive(std::unique_ptr<Event> e) override;

private:
    IPort& m_displayPort;
    IPort& m_foodPort;
    IPort& m_scorePort;

    std::pair<int, int> m_mapDimension;
    std::pair<int, int> m_foodPosition;

    std::unique_ptr<Segments> m_segments;

    void handleTimeoutInd();
    void handleDirectionInd(std::unique_ptr<Event>);
    void handleFoodInd(std::unique_ptr<Event>);
    void handleFoodResp(std::unique_ptr<Event>);
    void handlePauseInd(std::unique_ptr<Event>);

    void updateSegmentsIfSuccessfullMove(int x, int y);
    void addHeadSegment(int x, int y);
    void removeTailSegmentIfNotScored(int x, int y);
    void removeTailSegment();

    bool isPositionOutsideMap(int x, int y) const;

    void updateFoodPosition(int x, int y, std::function<void()> clearPolicy);
    void sendClearOldFood();
    void sendPlaceNewFood(int x, int y);

    bool m_paused;
};

} // namespace Snake
