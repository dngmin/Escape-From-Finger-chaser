#include "Entity.hpp"
#include <cassert>

Entity::Entity(const sf::Vector2f& init_pos, float rad, const sf::Color& color)
{
    assert(rad > 0 && "radは0より大きい数字である必要があります");
    this->position = init_pos;
    this->rad = rad;
    this->color = color;
    entity.setPosition({init_pos.x, init_pos.y});
    entity.setRadius(rad);
    entity.setOrigin({rad, rad});
    entity.setFillColor(color);
}