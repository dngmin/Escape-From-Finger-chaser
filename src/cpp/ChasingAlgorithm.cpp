#include "ChasingAlgorithm.hpp"
#include "Utils.hpp"

sf::Vector2f predictEulerPosition(const sf::Vector2f& curr_pos, const sf::Vector2f& curr_vel, float dT, int prediction_steps)
{
    return {
        curr_pos.x + curr_vel.x * (prediction_steps * dT),
        curr_pos.y + curr_vel.y * (prediction_steps * dT)
    };
}

// 速度ベクトルの成分のうち、移動量の大きい軸を優先した目標座標を取得
// 距離閾値に基づいた移動軸選択も同時に行う
sf::Vector2f getAxisAlignedPosition(const sf::Vector2f& curr_pos, const sf::Vector2f& curr_vel, const sf::Vector2f& chasing_object_pos, float distanceThreshold, const sf::Vector2u window_size)
{
    // 速度基準軸選定
    sf::Vector2f target_pos;
    bool x_move = std::abs(curr_vel.x) > std::abs(curr_vel.y);
    // 距離基準軸選定
    // 一般的にwidthの方が長いため、両方満たした時はx軸を優先する
    if (std::abs(curr_pos.y - chasing_object_pos.y) * window_size.y < distanceThreshold) x_move = true;
    else if (std::abs(curr_pos.x - chasing_object_pos.x) * window_size.x < distanceThreshold) x_move = false;
    
    target_pos.x = x_move? curr_pos.x : chasing_object_pos.x;
    target_pos.y = x_move? chasing_object_pos.y : curr_pos.y;

    return target_pos;
}

// 追跡オブジェクトの位置を基準とした4象限のなかプレイヤーのいるエリアに限ったランダム座標
sf::Vector2f getRandomPosition(const sf::Vector2f& player_pos, const sf::Vector2f& chasing_object_pos)
{
    sf::Vector2f max;
    sf::Vector2f min;
    sf::Vector2f random_pos;

    bool is_player_right = player_pos.x > chasing_object_pos.x;
    // 上下は反対であることに注意
    bool is_player_upper = player_pos.y < chasing_object_pos.y;

    max.x = is_player_right? 1.f : chasing_object_pos.x;
    min.x = is_player_right? chasing_object_pos.x : 0.f;
    max.y = is_player_upper? chasing_object_pos.y : 1.f;
    min.y = is_player_upper? 0.f : chasing_object_pos.y;

    random_pos.x = getRandomfloat(min.x, max.x);
    random_pos.y = getRandomfloat(min.y, max.y);
    return random_pos;
}