#pragma once

#include <cstdint>

#include "SnakePosition.hpp"

namespace Snake
{

enum Direction
{
    Direction_UP    = 0b00,
    Direction_DOWN  = 0b10,
    Direction_LEFT  = 0b01,
    Direction_RIGHT = 0b11
};

struct DirectionInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x10;

    Direction direction;
};


struct TimeoutInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x20;
};

enum Cell
{
    Cell_FREE,
    Cell_FOOD,
    Cell_SNAKE
};

struct DisplayInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x30;

    Position position;
    Cell value;
};

struct FoodInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x40;

    Position position;
};

struct FoodReq
{
    static constexpr std::uint32_t MESSAGE_ID = 0x41;
};

struct FoodResp
{
    static constexpr std::uint32_t MESSAGE_ID = 0x42;

    Position position;
};

struct ScoreInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x70;

    unsigned score;
};

struct LooseInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x71;
};

struct PauseInd
{
    static constexpr std::uint32_t MESSAGE_ID = 0x91;
};

} // namespace Snake
