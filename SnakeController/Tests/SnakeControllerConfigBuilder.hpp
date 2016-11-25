#include <sstream>

#include <vector>
#include <tuple>

#include "SnakeInterface.hpp"

namespace Snake
{

class ConfigBuilder
{
public:
    struct Point
    {
        unsigned int x;
        unsigned int y;
    };

    ConfigBuilder& setWorldSize(unsigned int x, unsigned int y)
    {
        worldX = x;
        worldY = y;

        return *this;
    }

    ConfigBuilder& setFood(unsigned int newFoodPositionX, unsigned int newFoodPositionY)
    {
        food.x = newFoodPositionX;
        food.y = newFoodPositionY;
        return *this;
    }

    ConfigBuilder& setDirection(Direction newDirection)
    {
        snakeDirection = newDirection;
        return *this;
    }

    ConfigBuilder& setSnake(std::vector<Point> newSnake)
    {
        snake = newSnake;
        return *this;
    }

    std::string convertDirection(Direction direction)
    {
        switch(direction)
        {
            case Direction::Direction_UP: return "U";
            case Direction::Direction_DOWN: return "D";
            case Direction::Direction_LEFT: return "L";
            case Direction::Direction_RIGHT: return "R";
            default:
                return "unknown direction";
        }
    }

    std::string build()
    {
        std::ostringstream buff;

        buff << "W " << worldX << " " << worldY << " F " << food.x << " " << food.y << " S " << convertDirection(snakeDirection) << " " << snake.size();

        for(const Point& point : snake)
        {
            buff << " " << point.x << " " << point.y;
        }

        return buff.str();
    }

private:
    unsigned int worldX = 100;
    unsigned int worldY = 100;
    Point food = Point{50, 50};
    Direction snakeDirection = Direction_LEFT;
    std::vector<Point> snake{Point{20,20}};
};

} // namespace Snake
