#include "Player.hpp"

Player::Player(const SimulationConfig::EntityConfig& entity_config)
    : entity(entity_config)
    , filter(1.f, 0.01f, 1.f)
    , T_prev(Clock::now())
{}

// 座標を0 ~ 1の値でもらうためWindowサイズで合わせる
void Player::update(const sf::Vector2f& received_pos, const sf::Vector2u& window_size)
{
    // 1€ filterの実装
    TimePoint T = Clock::now();
    dT = static_cast<float>(std::chrono::duration<double>(T - T_prev).count());
    curr_pos = filter.Filter(received_pos, dT);
    curr_vel = filter.getVelocity();

    // // 次の計算のため値変換
    T_prev = T;

    entity.entity.setPosition({curr_pos.x * window_size.x, curr_pos.y * window_size.y});
}