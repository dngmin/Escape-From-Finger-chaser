#pragma once
#include <SFML/Graphics.hpp>

// Euler法を用いたPlayerの予測位置取得
sf::Vector2f predictEulerPosition(const sf::Vector2f& curr_pos, const sf::Vector2f& curr_vel, float dT, int prediction_steps);

// 移動量の大きい軸を優先した目標座標を取得
sf::Vector2f getAxisAlignedPosition(const sf::Vector2f& curr_pos, const sf::Vector2f& curr_vel, const sf::Vector2f& chaser_pos, float distanceThreshold, const sf::Vector2u window_size);

// ランダム座標取得
sf::Vector2f getRandomPosition(const sf::Vector2f& player_pos, const sf::Vector2f& chaser_pos);