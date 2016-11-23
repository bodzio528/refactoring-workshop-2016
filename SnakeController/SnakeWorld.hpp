#pragma once

#include <utility>

namespace Snake
{

class World
{
public:
    World(std::pair<int, int> dimension, std::pair<int, int> food);

    void setFoodPosition(std::pair<int, int> position);
    std::pair<int, int> getFoodPosition() const;

    bool contains(int x, int y) const;

private:
    std::pair<int, int> m_foodPosition;
    std::pair<int, int> m_dimension;
};

} // namespace Snake
