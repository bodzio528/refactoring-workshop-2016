#include "SnakeController.hpp"

#include "EventT.hpp"

#include <gtest/gtest.h>

#include "Mocks/PortMock.hpp"
#include "Mocks/EventMatchers.hpp"

using namespace ::testing;

namespace Snake
{

struct SnakeTest : Test
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

TEST_F(SnakeTest, test_EmptyConfig_ThrowsException)
{
    EXPECT_THROW(configureSUT(""), ConfigurationError);
}

TEST_F(SnakeTest, test_LackOfControlLetters_ThrowsException_MissingW)
{
    EXPECT_THROW(configureSUT("X 100 100"), ConfigurationError);
}

TEST_F(SnakeTest, test_LackOfControlLetters_ThrowsException_MissingF)
{
    EXPECT_THROW(configureSUT("W 100 100 X 50 50"), ConfigurationError);
}

TEST_F(SnakeTest, test_LackOfControlLetters_ThrowsException_MissingS)
{
    EXPECT_THROW(configureSUT("W 100 100 F 50 50 X"), ConfigurationError);
}

TEST_F(SnakeTest, test_LackOfSnakeDirection_ThrowsException)
{
    EXPECT_THROW(configureSUT("W 100 100 F 50 50 S X"), ConfigurationError);
}

TEST_F(SnakeTest, test_UnexpectedEvent_ThrowsException)
{
    configureSUT("W 100 100 F 50 50 S U 1 20 20");
    EXPECT_THROW(sut->receive(std::make_unique<EventT<DisplayInd>>()), UnexpectedEventException);
}

struct SnakeDirectionsTest : SnakeTest
{
    std::string snakeU = "W 100 100 F 50 50 S U 1 20 20";
    std::string snakeD = "W 100 100 F 50 50 S D 1 20 20";
    std::string snakeR = "W 100 100 F 50 50 S R 1 20 20";
    std::string snakeL = "W 100 100 F 50 50 S L 1 20 20";
};

TEST_F(SnakeDirectionsTest, test_snakeDirectionDown_AddsSegmentWithIncreasedY)
{
    configureSUT(snakeD);

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 20, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 21, Cell_SNAKE)));

    sut->receive(te.clone());
}

TEST_F(SnakeDirectionsTest, test_snakeDirectionUp_AddsSegmentWithDecreasedY)
{
    configureSUT(snakeU);

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 20, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 19, Cell_SNAKE)));

    sut->receive(te.clone());
}

TEST_F(SnakeDirectionsTest, test_snakeDirectionRight_AddsSegmentWithIncreasedX)
{
    configureSUT(snakeR);

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 20, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(21, 20, Cell_SNAKE)));

    sut->receive(te.clone());
}

TEST_F(SnakeDirectionsTest, test_snakeDirectionLeft_AddsSegmentWithDecreasedX)
{
    configureSUT(snakeL);

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 20, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(19, 20, Cell_SNAKE)));

    sut->receive(te.clone());
}

struct SnakeDirectionChangeTest : SnakeTest
{
    EventT<DirectionInd> toDown;
    EventT<DirectionInd> toLeft;
    EventT<DirectionInd> toRight;

    void SetUp() override
    {
        toDown->direction = Direction_DOWN;
        toLeft->direction = Direction_LEFT;
        toRight->direction = Direction_RIGHT;

        configureSUT("W 100 100 F 50 50 S U 1 20 20");
    }
};

TEST_F(SnakeDirectionChangeTest, test_perpendicularDirectionChangeIsAllowed)
{
    sut->receive(toRight.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 20, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(21, 20, Cell_SNAKE)));

    sut->receive(te.clone());
}

TEST_F(SnakeDirectionChangeTest, test_parallelDirectionChangeIsForbidden)
{
    sut->receive(toDown.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 20, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 19, Cell_SNAKE)));

    sut->receive(te.clone());
}

TEST_F(SnakeDirectionChangeTest, test_directionChangesAreNotEnqueued)
{
    sut->receive(toRight.clone());
    sut->receive(toDown.clone());
    sut->receive(toLeft.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(20, 20, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(19, 20, Cell_SNAKE)));

    sut->receive(te.clone());
}

struct SnakeMoveTest : SnakeTest
{
    void SetUp() override
    {
        configureSUT("W 100 100 F 50 50 S R 5 20 20 19 20 18 20 17 20 16 20");
    }
};

TEST_F(SnakeMoveTest, test_afterTimerEvents_SnakeMoves)
{
    Sequence l_freeSeq, l_takeSeq;

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(16, 20, Cell_FREE))).InSequence(l_freeSeq);
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(21, 20, Cell_SNAKE))).InSequence(l_takeSeq);
    sut->receive(te.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(17, 20, Cell_FREE))).InSequence(l_freeSeq);
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(22, 20, Cell_SNAKE))).InSequence(l_takeSeq);
    sut->receive(te.clone());

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(18, 20, Cell_FREE))).InSequence(l_freeSeq);
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(23, 20, Cell_SNAKE))).InSequence(l_takeSeq);
    sut->receive(te.clone());
}

struct SnakeBorderTest : SnakeTest
{
    std::string snakeU = "W 100 100 F 50 50 S U 1 50  0";
    std::string snakeD = "W 100 100 F 50 50 S D 1 50 99";
    std::string snakeL = "W 100 100 F 50 50 S L 1  0 50";
    std::string snakeR = "W 100 100 F 50 50 S R 1 99 50";
};

TEST_F(SnakeBorderTest, test_ReachingUpperBorder_SendLooseInd)
{
    configureSUT(snakeU);

    EXPECT_CALL(scorePortMock, send_rvr(AnyLooseInd()));

    sut->receive(te.clone());
}

TEST_F(SnakeBorderTest, test_ReachingLowerBorder_SendLooseInd)
{
    configureSUT(snakeD);

    EXPECT_CALL(scorePortMock, send_rvr(AnyLooseInd()));

    sut->receive(te.clone());
}

TEST_F(SnakeBorderTest, test_ReachingLeftBorder_SendLooseInd)
{
    configureSUT(snakeL);

    EXPECT_CALL(scorePortMock, send_rvr(AnyLooseInd()));

    sut->receive(te.clone());
}

TEST_F(SnakeBorderTest, test_ReachingRightBorder_SendLooseInd)
{
    configureSUT(snakeR);

    EXPECT_CALL(scorePortMock, send_rvr(AnyLooseInd()));

    sut->receive(te.clone());
}

struct SnakeTailbitingTest : SnakeTest
{
    void SetUp() override
    {
        configureSUT("W 100 100 F 50 50 S L 9 20 20 21 20 21 21 21 22 20 22 19 22 19 21 19 20 19 19");
    }
};

TEST_F(SnakeTailbitingTest, test_WhenBitesOwnTail_GameIsLost)
{
    EXPECT_CALL(scorePortMock, send_rvr(AnyLooseInd()));

    sut->receive(te.clone());
}


struct SnakeEatTestSuite : SnakeTest
{
    void SetUp() override
    {
        configureSUT("W 100 100 F 21 20 S R 1 20 20");
    }
};

TEST_F(SnakeEatTestSuite, test_IfFoodEncountered_SendFoodRequestAndScoreIndication)
{
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(21, 20, Cell_SNAKE)));
    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));
    EXPECT_CALL(scorePortMock, send_rvr(AnyScoreInd()));

    sut->receive(te.clone());
}

TEST_F(SnakeEatTestSuite, test_ReceiveFoodResp_PlaceFoodInCell)
{
    FoodResp l_foodResp;
    l_foodResp.x = 50;
    l_foodResp.y = 50;

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(50, 50, Cell_FOOD)));

    sut->receive(std::make_unique<EventT<FoodResp>>(l_foodResp));
}

TEST_F(SnakeEatTestSuite, test_ReceiveFoodRespDetectsCollision_ThenRequestNewFood)
{
    FoodResp l_foodResp;
    l_foodResp.x = 20;
    l_foodResp.y = 20;

    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));

    sut->receive(std::make_unique<EventT<FoodResp>>(l_foodResp));
}

struct SnakeNewFoodTest : SnakeTest
{
    void SetUp() override
    {
        configureSUT("W 100 100 F 50 50 S R 1 20 20");
    }
};

TEST_F(SnakeNewFoodTest, test_ReceiveFoodInd_ClearOldFoodAndPlaceNewOne)
{
    FoodInd l_foodInd;
    l_foodInd.x = 30;
    l_foodInd.y = 30;

    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(50, 50, Cell_FREE)));
    EXPECT_CALL(displayPortMock, send_rvr(DisplayIndEq(30, 30, Cell_FOOD)));

    sut->receive(std::make_unique<EventT<FoodInd>>(l_foodInd));
}

TEST_F(SnakeNewFoodTest, test_ReceiveFoodIndDetectsCollision_ThenRequestNewFood)
{
    FoodInd l_foodInd;
    l_foodInd.x = 20;
    l_foodInd.y = 20;

    EXPECT_CALL(foodPortMock, send_rvr(AnyFoodReq()));

    sut->receive(std::make_unique<EventT<FoodInd>>(l_foodInd));
}
} // namespace Snake
