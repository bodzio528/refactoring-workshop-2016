#include "SnakeTorusWorld.hpp"

#include "IPort.hpp"
#include "EventT.hpp"

#include "SnakeInterface.hpp"
#include "SnakeSegments.hpp"


namespace Snake
{

boost::optional<Position> TorusWorld::tryWalk(Position position) const
{
    if (position.x == -1) {
        position.x = m_dimension.width - 1;
    }
    if (position.y == -1) {
        position.y = m_dimension.height - 1;
    }
    position.x %= m_dimension.width;
    position.y %= m_dimension.height;

    return boost::make_optional(true, position);
}

} // namespace Snake
