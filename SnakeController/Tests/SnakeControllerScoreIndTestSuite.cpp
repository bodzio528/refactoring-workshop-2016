#include "SnakeController.hpp"

#include "EventT.hpp"

#include <gtest/gtest.h>

#include "Mocks/PortMock.hpp"
#include "Mocks/EventMatchers.hpp"

using namespace ::testing;

namespace Snake
{

struct SnakeScoreTest : Test
{
    EventT<TimeoutInd> te;

    StrictMock<PortMock> displayPortMock;
    StrictMock<PortMock> foodPortMock;
    StrictMock<PortMock> scorePortMock;

    void configureSUT(std::string p_config)
    {
        sut = std::make_unique<Controller>(displayPortMock, foodPortMock, scorePortMock, p_config);
    }

    std::unique_ptr<Controller> sut = nullptr;
};

TEST_F(SnakeScoreTest, snakeOneSegmentLongShouldEmitScoreWith1)
{
    configureSUT("W 100 100 F 21 20 S R 1 20 20");

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(21, 20, Cell_SNAKE)));
    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));
    EXPECT_CALL(scorePortMock, send_rvr(ScoreIndEq(1)));

    sut->receive(te.clone());
}
TEST_F(SnakeScoreTest, snakeThreeSegmentsLongShouldEmitScoreWith3)
{
    configureSUT("W 100 100 F 21 20 S R 3 20 20 19 20 18 20");

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(21, 20, Cell_SNAKE)));
    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));
    EXPECT_CALL(scorePortMock, send_rvr(ScoreIndEq(3)));

    sut->receive(te.clone());
}


} // namespace Snake
