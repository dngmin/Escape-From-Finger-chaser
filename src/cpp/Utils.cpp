#include "Utils.hpp"

float getDistance(const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2u& window_size)
{
    float diff_x = (A.x - B.x) * window_size.x;
    float diff_y = (A.y - B.y) * window_size.y;
    return std::sqrt(Square(diff_x) + Square(diff_y));
}