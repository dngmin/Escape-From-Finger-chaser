#pragma once
#include <SFML/Graphics.hpp>

// Euler法を用いたPlayerの予測位置取得
const sf::Vector2f predictEulerPosition(const sf::Vector2f& curr_pos, const sf::Vector2f& curr_vel, float dT, int prediction_steps)
{
    return {
        curr_pos.x + curr_vel.x * (prediction_steps * dT),
        curr_pos.y + curr_vel.y * (prediction_steps * dT)
    };
}