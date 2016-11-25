#include "SnakeController.hpp"

#include <algorithm>
#include <sstream>

#include "EventT.hpp"
#include "IPort.hpp"

#include "SnakeSegments.hpp"
#include "SnakeWorld.hpp"
#include "SnakeTorusWorld.hpp"

namespace Snake
{

ConfigurationError::ConfigurationError()
    : std::logic_error("Bad configuration of Snake::Controller.")
{}

UnexpectedEventException::UnexpectedEventException()
    : std::runtime_error("Unexpected event received!")
{}

bool checkControl(std::istream& istr, char control)
{
    char input;
    istr >> input;
    return input == control;
}

Dimension readWorldDimension(std::istream& istr)
{
    Dimension dimension;
    istr >> dimension.width >> dimension.height;
    return dimension;
}

Position readFoodPosition(std::istream& istr)
{
    if (not checkControl(istr, 'F')) {
        throw ConfigurationError();
    }

    Position position;
    istr >> position.x >> position.y;
    return position;
}

std::unique_ptr<World> readWorld(std::istream& istr, IPort& displayPort, IPort& foodPort)
{
    char worldType;
    istr >> worldType;

    auto worldDimension = readWorldDimension(istr);
    auto foodPosition = readFoodPosition(istr);

    switch (worldType) {
        case 'W':
            return std::make_unique<World>(displayPort, foodPort, worldDimension, foodPosition);
        case 'T':
            return std::make_unique<TorusWorld>(displayPort, foodPort, worldDimension, foodPosition);
        default:
            throw ConfigurationError();
    }
}

Direction readDirection(std::istream& istr)
{
    if (not checkControl(istr, 'S')) {
        throw ConfigurationError();
    }

    char direction;
    istr >> direction;
    switch (direction) {
        case 'U':
            return Direction_UP;
        case 'D':
            return Direction_DOWN;
        case 'L':
            return Direction_LEFT;
        case 'R':
            return Direction_RIGHT;
        default:
            throw ConfigurationError();
    }
}

Controller::Controller(IPort& displayPort, IPort& foodPort, IPort& scorePort, std::string const& initialConfiguration)
    : m_paused(false)
{
    std::istringstream istr(initialConfiguration);

    m_world = readWorld(istr, displayPort, foodPort);
    m_segments = std::make_unique<Segments>(displayPort, scorePort, readDirection(istr));

    int length;
    istr >> length;

    while (length--) {
        Position position;
        istr >> position.x >> position.y;
        m_segments->addSegment(position);
    }

    if (length != -1) {
        throw ConfigurationError();
    }
}

Controller::~Controller()
{}

void Controller::handleTimeoutInd()
{
    m_segments->nextStep(*m_world);
}

void Controller::handleDirectionInd(std::unique_ptr<Event> e)
{
    m_segments->updateDirection(payload<DirectionInd>(*e).direction);
}

void Controller::handleFoodInd(std::unique_ptr<Event> e)
{
    m_world->updateFoodPosition(payload<FoodResp>(*e).position, *m_segments);
}

void Controller::handleFoodResp(std::unique_ptr<Event> e)
{
    m_world->placeFood(payload<FoodResp>(*e).position, *m_segments);
}

void Controller::handlePauseInd(std::unique_ptr<Event> e)
{
    m_paused = not m_paused;
}

void Controller::receive(std::unique_ptr<Event> e)
{
    switch (e->getMessageId()) {
        case TimeoutInd::MESSAGE_ID:
            if (!m_paused) {
                return handleTimeoutInd();
            }
            return;
        case DirectionInd::MESSAGE_ID:
            if (!m_paused) {
                return handleDirectionInd(std::move(e));
            }
            return;
        case FoodInd::MESSAGE_ID:
            return handleFoodInd(std::move(e));
        case FoodResp::MESSAGE_ID:
            return handleFoodResp(std::move(e));
        case PauseInd::MESSAGE_ID:
            return handlePauseInd(std::move(e));
        default:
            throw UnexpectedEventException();
    }
}

} // namespace Snake
