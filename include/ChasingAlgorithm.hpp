#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

// Euler法を用いたPlayerの予測位置取得
const sf::Vector2f predictEulerPosition(const sf::Vector2f& curr_pos, const sf::Vector2f& curr_vel, float dT, int prediction_steps)
{
    return {
        curr_pos.x + curr_vel.x * (prediction_steps * dT),
        curr_pos.y + curr_vel.y * (prediction_steps * dT)
    };
}

// ランダム座標取得
// チェーサーの位置を基準とした4象限のなかプレイヤーのいるエリアに限ったランダム座標
const sf::Vector2f getRandomPosition(const sf::Vector2f& player_pos, const sf::Vector2f& chaser_pos)
{
    sf::Vector2f max;
    sf::Vector2f min;
    sf::Vector2f random_pos;

    bool is_player_right = player_pos.x > chaser_pos.x;
    // 上下は反対であることに注意
    bool is_player_upper = player_pos.y < chaser_pos.y;

    max.x = is_player_right? 1.f : chaser_pos.x;
    min.x = is_player_right? chaser_pos.x : 0.f;
    max.y = is_player_upper? chaser_pos.y : 1.f;
    min.y = is_player_upper? 0.f : chaser_pos.y;

    random_pos.x = getRandomfloat(min.x, max.x);
    random_pos.y = getRandomfloat(min.y, max.y);
    return random_pos;
}