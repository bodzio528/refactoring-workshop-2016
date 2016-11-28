#include "SnakeController.hpp"

#include "EventT.hpp"

#include <gtest/gtest.h>

#include "Mocks/PortMock.hpp"
#include "Mocks/EventMatchers.hpp"

#include "SnakeControllerConfigBuilder.hpp"

using namespace ::testing;

namespace Snake
{

struct TorusSnakeTest : Test
{
    using Points = std::vector<ConfigBuilder::Point>;
    EventT<TimeoutInd> te;

    StrictMock<PortMock> displayPortMock;
    StrictMock<PortMock> foodPortMock;
    StrictMock<PortMock> scorePortMock;

    void configureSUT(std::string p_config)
    {
        sut = std::make_unique<Controller>(displayPortMock, foodPortMock, scorePortMock, p_config);
    }

    std::unique_ptr<Controller> sut = nullptr;

    void moveTime()
    {
        sut->receive(te.clone());
    }

    void changeSnakeDirection(Direction newDirection)
    {
        EventT<DirectionInd> directionInd(DirectionInd{newDirection});

        sut->receive(directionInd.clone());
    }

    void sendFoodResp(int x, int y)
    {
        EventT<FoodResp> foodResp(FoodResp{x, y});

        sut->receive(foodResp.clone());
    }
};

TEST_F(TorusSnakeTest, inTorusMode_SnakeDoesNotTouchBorders_ShouldMoveAsInFlatMode)
{
    configureSUT(ConfigBuilder().setWorldType(WorldType::TORUS).setWorldSize(4, 4)
                                .setFood(0, 0)
                                .setDirection(Direction_DOWN)
                                .setSnake(std::vector<ConfigBuilder::Point>{{1,1}}).build());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(1,1, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(1,2, Cell_SNAKE)));

    moveTime();

    changeSnakeDirection(Direction_RIGHT);

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(1,2, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2,2, Cell_SNAKE)));

    moveTime();

    changeSnakeDirection(Direction_UP);

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2,2, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2,1, Cell_SNAKE)));

    moveTime();

    changeSnakeDirection(Direction_LEFT);

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2,1, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(1,1, Cell_SNAKE)));

    moveTime();
}

TEST_F(TorusSnakeTest, inTorusMode_SnakeDoesNotTouchBorders_GameIsOverWhenSnakeBiteItself)
{
    configureSUT(ConfigBuilder().setWorldType(WorldType::TORUS).setWorldSize(4, 4)
                                .setFood(0, 0)
                                .setDirection(Direction_RIGHT)
                                .setSnake(Points{{1,1},{1,2},{2,2},{2,1}}).build());

    EXPECT_CALL(scorePortMock, send_rvr(AnyLooseInd()));

    moveTime();
}

TEST_F(TorusSnakeTest, inTorusMode_SnakeDoesNotTouchBorders_SnakeScoresWhenFoodGetsEaten)
{
    configureSUT(ConfigBuilder().setWorldType(WorldType::TORUS).setWorldSize(4, 4)
                                .setFood(2, 1)
                                .setDirection(Direction_RIGHT)
                                .setSnake(Points{{1,1}}).build());

    EXPECT_CALL(scorePortMock, send_rvr(AnyScoreInd()));
    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2,1, Cell_SNAKE)));

    moveTime();
}

TEST_F(TorusSnakeTest, inTorusMode_FoodRespHandlingSetsNewPlaceForFood)
{
    configureSUT(ConfigBuilder().setWorldType(WorldType::TORUS).setWorldSize(4, 4)
                                .setFood(1, 1)
                                .setDirection(Direction_RIGHT)
                                .setSnake(Points{{2,1}}).build());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(3,1, Cell_FOOD)));
    sendFoodResp(3,1);

    EXPECT_CALL(scorePortMock, send_rvr(AnyScoreInd()));
    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(3,1, Cell_SNAKE)));

    moveTime();
}

TEST_F(TorusSnakeTest, inTorusMode_FoodInTheSameRowButOtherColumn)
{
    configureSUT(ConfigBuilder().setWorldType(WorldType::TORUS).setWorldSize(4, 4)
                                .setFood(1, 1)
                                .setDirection(Direction_UP)
                                .setSnake(Points{{2,2}}).build());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2,2, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2,1, Cell_SNAKE)));

    moveTime();
}

TEST_F(TorusSnakeTest, inTorusMode_FoodInTheSameCulmnButOtherRow)
{
    configureSUT(ConfigBuilder().setWorldType(WorldType::TORUS).setWorldSize(4, 4)
                                .setFood(1, 1)
                                .setDirection(Direction_LEFT)
                                .setSnake(Points{{2,2}}).build());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(2,2, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(1,2, Cell_SNAKE)));

    moveTime();
}

struct ThroughTheEdgeParams
{
    std::vector<ConfigBuilder::Point> snake;
    ConfigBuilder::Point newHead;
    Direction snakeDirection;
};

struct TorusSnakeThroughTheEdge : TorusSnakeTest,
                                  public WithParamInterface<ThroughTheEdgeParams>
{
};

TEST_P(TorusSnakeThroughTheEdge, inTorusMode_NothingOverTheEdge_SnakeShouldMoveOverTheEdge)
{
    configureSUT(ConfigBuilder().setWorldType(WorldType::TORUS).setWorldSize(3, 3)
                                .setFood(1, 1)
                                .setDirection(GetParam().snakeDirection)
                                .setSnake(GetParam().snake).build());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(GetParam().snake.front().x,
                                                       GetParam().snake.front().y, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(GetParam().newHead.x,
                                                       GetParam().newHead.y, Cell_SNAKE)));

    moveTime();
}

TEST_P(TorusSnakeThroughTheEdge, inTorusMode_FoodOverTheEdge_SnakeShouldMoveOverTheEdgeAndScore)
{
    configureSUT(ConfigBuilder().setWorldType(WorldType::TORUS).setWorldSize(3, 3)
                                .setFood(GetParam().newHead.x, GetParam().newHead.y)
                                .setDirection(GetParam().snakeDirection)
                                .setSnake(GetParam().snake).build());

    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));
    EXPECT_CALL(scorePortMock, send_rvr(AnyScoreInd()));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(GetParam().newHead.x,
                                                       GetParam().newHead.y, Cell_SNAKE)));

    moveTime();
}

TEST_P(TorusSnakeThroughTheEdge, inTorusMode_SnakeOverTheEdge_SnakeShouldMoveOverTheEdgeAndLoose)
{
    auto snake = GetParam().snake;
    snake.push_back(GetParam().newHead);

    configureSUT(ConfigBuilder().setWorldType(WorldType::TORUS).setWorldSize(3, 3)
                                .setFood(1, 1)
                                .setDirection(GetParam().snakeDirection)
                                .setSnake(snake).build());

    EXPECT_CALL(scorePortMock, send_rvr(AnyLooseInd()));

    moveTime();
}

INSTANTIATE_TEST_CASE_P(TorusSnakeThroughTheEdgeInstantation,
                        TorusSnakeThroughTheEdge,
                        Values(ThroughTheEdgeParams{{{0,0}},{0,2}, Direction_UP},
                               ThroughTheEdgeParams{{{1,2}},{1,0}, Direction_DOWN},
                               ThroughTheEdgeParams{{{2,2}},{0,2}, Direction_RIGHT},
                               ThroughTheEdgeParams{{{0,1}},{2,1}, Direction_LEFT}));

} // namespace Snake
