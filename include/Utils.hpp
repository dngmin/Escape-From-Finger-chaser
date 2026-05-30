#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

auto Square(auto x)
{
    return x * x;
}

float getDistance(const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2u& window_size = {1,1});