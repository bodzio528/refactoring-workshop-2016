#pragma once

#include "EventT.hpp"
#include "SnakeInterface.hpp"

#include <gmock/gmock.h>

namespace Snake
{

MATCHER_P3(DisplayIndEq, p_x, p_y, p_value, "")
try {
    auto const& l_msg = payload<DisplayInd>(arg);
    *result_listener << "carrying PaintReq(" << l_msg.position.x << ", " << l_msg.position.y << ", " << l_msg.value << ")";
    return l_msg.position.x == p_x and l_msg.position.y == p_y and l_msg.value == p_value;
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

MATCHER_P(ScoreIndEq, p_score, "")
{
    *result_listener << "message with id = 0x" << std::hex << arg.getMessageId();
    auto const& l_msg = payload<ScoreInd>(arg);
    return ScoreInd::MESSAGE_ID == arg.getMessageId() and l_msg.score == p_score;
}

MATCHER(AnyFoodReq, "")
{
    *result_listener << "message with id = 0x" << std::hex << arg.getMessageId();
    return FoodReq::MESSAGE_ID == arg.getMessageId();
}

} // namespace Snake
