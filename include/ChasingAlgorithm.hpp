#pragma once
#include <SFML/Graphics.hpp>

// Euler法を用いたPlayerの予測位置取得
sf::Vector2f predictEulerPosition(const sf::Vector2f& curr_pos, const sf::Vector2f& curr_vel, float dT, int prediction_steps);

// ランダム座標取得
sf::Vector2f getRandomPosition(const sf::Vector2f& player_pos, const sf::Vector2f& chaser_pos);