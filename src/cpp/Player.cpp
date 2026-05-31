#include "Player.hpp"

Player::Player(const GameConfig::EntityConfig& entity_config)
    : state(entity_config.init_pos)
    , entity(entity_config)
{}

// 座標を0 ~ 1の値でもらうためWindowサイズで合わせる
void Player::update(const sf::Vector2f& received_pos, const sf::Vector2u& window_size)
{
    state.update(received_pos);
    sf::Vector2f curr_pos = state.get_curr_pos();
    entity.entity.setPosition({curr_pos.x * window_size.x, curr_pos.y * window_size.y});
}

sf::Vector2f Player::getPosition() const
{
    return state.get_curr_pos();
}

sf::Vector2f Player::getEulerPredict() const
{
    return state.get_Euler_predict();
}