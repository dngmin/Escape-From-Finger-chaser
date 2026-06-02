#include "Chaser.hpp"

Chaser::Chaser(const GameConfig::EntityConfig& entity_config)
    : state(entity_config.init_pos)
    , entity(entity_config)
{}

// 座標を0 ~ 1の値でもらうためWindowサイズで合わせる
void Chaser::update(const sf::Vector2f& target_pos, const sf::Vector2u& window_size, const float speed)
{
    state.update(target_pos, speed);
    sf::Vector2f curr_pos = state.get_curr_pos();
    entity.entity.setPosition({curr_pos.x * window_size.x, curr_pos.y * window_size.y});
}

sf::Vector2f Chaser::getPosition() const
{
    return state.get_curr_pos();
}