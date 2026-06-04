#include "Utils.hpp"
#include <random>

const float getDistance(const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2u& window_size)
{
    float diff_x = (A.x - B.x) * window_size.x;
    float diff_y = (A.y - B.y) * window_size.y;
    return std::sqrt(Square(diff_x) + Square(diff_y));
}

const float getRandomfloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}