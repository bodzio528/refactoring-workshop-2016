#include "SnakeController.hpp"

#include "EventT.hpp"

#include <gtest/gtest.h>

#include "Mocks/PortMock.hpp"
#include "Mocks/EventMatchers.hpp"

#include "SnakeControllerConfigBuilder.hpp"

using namespace ::testing;

namespace Snake
{

struct PauseSnakeTest : Test
{
    EventT<TimeoutInd> te;
    EventT<PauseInd> pauseEvent;

    StrictMock<PortMock> displayPortMock;
    StrictMock<PortMock> foodPortMock;
    StrictMock<PortMock> scorePortMock;

    void configureSUT(std::string p_config)
    {
        sut = std::make_unique<Controller>(displayPortMock, foodPortMock, scorePortMock, p_config);
    }

    std::unique_ptr<Controller> sut = nullptr;
};

TEST_F(PauseSnakeTest, pauseEventShouldNotBeUnexpected)
{
    configureSUT(ConfigBuilder().build());

    sut->receive(pauseEvent.clone());
    sut->receive(pauseEvent.clone());
}

TEST_F(PauseSnakeTest, whenPausedSnakeShouldNotMove)
{
    configureSUT(ConfigBuilder().setDirection(Direction_RIGHT).setSnake({ConfigBuilder::Point{20, 20}}).build());

    sut->receive(pauseEvent.clone());
    sut->receive(te.clone());
}

TEST_F(PauseSnakeTest, whenNoLongerPausedSnakeShouldMoveAgain)
{
    configureSUT(ConfigBuilder().setDirection(Direction_RIGHT).setSnake({ConfigBuilder::Point{20, 20}}).build());

    sut->receive(pauseEvent.clone());
    sut->receive(pauseEvent.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 20, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(21, 20, Cell_SNAKE)));

    sut->receive(te.clone());
}

TEST_F(PauseSnakeTest, whenPausedSnakeShouldNotHitTheWall)
{
    configureSUT(ConfigBuilder().setWorldSize(5,5).setFood(1,1)
                 .setDirection(Direction_RIGHT).setSnake({ConfigBuilder::Point{5, 5}}).build());

    sut->receive(pauseEvent.clone());
    sut->receive(te.clone());
}

TEST_F(PauseSnakeTest, whenPausedSnakeShouldNotBiteItself)
{
    configureSUT(ConfigBuilder().setWorldSize(5,5).setFood(1,1).setDirection(Direction_RIGHT)
                 .setSnake({ConfigBuilder::Point{3, 3},ConfigBuilder::Point{3,2},
                            ConfigBuilder::Point{2,2}, ConfigBuilder::Point{2,3}}).build());

    sut->receive(pauseEvent.clone());
    sut->receive(te.clone());
}

TEST_F(PauseSnakeTest, whenPausedDirectionChangeRequestsShouldBeIgnored)
{
    configureSUT(ConfigBuilder().setWorldSize(5,5).setFood(1,1).setDirection(Direction_RIGHT)
                 .setSnake({ConfigBuilder::Point{2,2}}).build());

    sut->receive(pauseEvent.clone());

    EventT<DirectionInd> directionChange(DirectionInd{Direction_UP});
    sut->receive(directionChange.clone());

    sut->receive(pauseEvent.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2, 2, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(3, 2, Cell_SNAKE)));

    sut->receive(te.clone());
}

TEST_F(PauseSnakeTest, whenPausedFoodPositionChangeShouldBeStillProcessed)
{
    configureSUT(ConfigBuilder().setWorldSize(5,5).setFood(3,2).setDirection(Direction_RIGHT)
                 .setSnake({ConfigBuilder::Point{2,2}}).build());

    sut->receive(pauseEvent.clone());
    EventT<FoodInd> foodPositionChange(FoodInd{4,2});

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(3,2, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(4,2, Cell_FOOD)));
    sut->receive(foodPositionChange.clone());

    sut->receive(pauseEvent.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2, 2, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(3, 2, Cell_SNAKE)));

    sut->receive(te.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(4, 2, Cell_SNAKE)));
    EXPECT_CALL(scorePortMock, send_rvr(AnyScoreInd()));
    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));

    sut->receive(te.clone());
}

TEST_F(PauseSnakeTest, whenPausedFoodPositionChangeShouldBeStillProcessed_FoodRespCase)
{
    configureSUT(ConfigBuilder().setWorldSize(5,5).setFood(3,2).setDirection(Direction_RIGHT)
                 .setSnake({ConfigBuilder::Point{2,2}}).build());

    sut->receive(pauseEvent.clone());

    EventT<FoodResp> foodPositionChange(FoodResp{4,2});

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(4,2, Cell_FOOD)));
    sut->receive(foodPositionChange.clone());

    sut->receive(pauseEvent.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2, 2, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(3, 2, Cell_SNAKE)));

    sut->receive(te.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(4, 2, Cell_SNAKE)));
    EXPECT_CALL(scorePortMock, send_rvr(AnyScoreInd()));
    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));

    sut->receive(te.clone());
}

} // namespace Snake
