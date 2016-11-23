#pragma once

#include "EventT.hpp"
#include "SnakeInterface.hpp"

#include <gmock/gmock.h>

namespace Snake
{

MATCHER_P3(DisplayIndEq, p_x, p_y, p_value, "")
try {
    auto const& l_msg = payload<DisplayInd>(arg);
    *result_listener << "carrying PaintReq(" << l_msg.x << ", " << l_msg.y << ", " << l_msg.value << ")";
    return l_msg.x == p_x and l_msg.y == p_y and l_msg.value == p_value;
} catch (std::bad_cast& l_exc) {
    *result_listener << "not carrying PaintReq at all.";
    return false;
}

MATCHER(AnyLooseInd, "")
{
    *result_listener << "message with id = 0x" << std::hex << arg.getMessageId();
    return LooseInd::MESSAGE_ID == arg.getMessageId();
}

MATCHER(AnyScoreInd, "")
{
    *result_listener << "message with id = 0x" << std::hex << arg.getMessageId();
    return ScoreInd::MESSAGE_ID == arg.getMessageId();
}

MATCHER(AnyFoodReq, "")
{
    *result_listener << "message with id = 0x" << std::hex << arg.getMessageId();
    return FoodReq::MESSAGE_ID == arg.getMessageId();
}

} // namespace Snake
