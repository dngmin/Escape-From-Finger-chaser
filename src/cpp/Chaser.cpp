#include "Chaser.hpp"

Chaser::Chaser(const sf::Vector2f& init_pos, float rad, const sf::Color& color)
    : state(init_pos)
    , entity(init_pos, rad, color)
{}

// 座標を0 ~ 1の値でもらうためWindowサイズで合わせる
void Chaser::update(sf::Vector2f target_pos, sf::Vector2u window_size)
{
    state.update(target_pos);
    sf::Vector2f curr_pos = state.get_curr_pos();
    entity.entity.setPosition({curr_pos.x * window_size.x, curr_pos.y * window_size.y});
}

sf::Vector2f Chaser::getPosition() const
{
    return state.get_curr_pos();
}