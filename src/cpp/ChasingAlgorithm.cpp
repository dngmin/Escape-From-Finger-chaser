#include "ChasingAlgorithm.hpp"
#include "Utils.hpp"
#include <cmath>

sf::Vector2f predictEulerPosition(const sf::Vector2f& curr_pos, const sf::Vector2f& curr_vel, float dT, int prediction_steps)
{
    return {
        curr_pos.x + curr_vel.x * (prediction_steps * dT),
        curr_pos.y + curr_vel.y * (prediction_steps * dT)
    };
}

// 速度ベクトルの成分のうち、移動量の大きい軸を優先した目標座標を取得
// 距離閾値に基づいた移動軸選択も同時に行う
sf::Vector2f getAxisAlignedPosition(const sf::Vector2f& curr_pos, const sf::Vector2f& curr_vel, const sf::Vector2f& chaser_pos, float distanceThreshold, const sf::Vector2u window_size)
{
    sf::Vector2f target_pos;
    bool x_move = std::abs(curr_vel.x) > std::abs(curr_vel.y);
    // 距離閾値超過確認
    if (std::abs(curr_pos.y - chaser_pos.y) * window_size.x < distanceThreshold) x_move = true;
    else if (std::abs(curr_pos.x - chaser_pos.x) * window_size.y < distanceThreshold) x_move = false;
    
    target_pos.x = x_move? curr_pos.x : chaser_pos.x;
    target_pos.y = x_move? chaser_pos.y : curr_pos.y;

    return target_pos;
}

// チェーサーの位置を基準とした4象限のなかプレイヤーのいるエリアに限ったランダム座標
sf::Vector2f getRandomPosition(const sf::Vector2f& player_pos, const sf::Vector2f& chaser_pos)
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