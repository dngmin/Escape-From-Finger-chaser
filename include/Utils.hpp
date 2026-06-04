#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

const auto Square(auto x)
{
    return x * x;
}

const float getDistance(const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2u& window_size = {1,1});

const float getRandomfloat(float min, float max);