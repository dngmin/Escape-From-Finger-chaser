#include "Entity.hpp"
#include <cassert>

Entity::Entity(const GameConfig::EntityConfig entity_config)
{
    assert(entity_config.entity_size > 0 && "Entityサイズは0より大きい数字である必要があります");
    this->position = entity_config.init_pos;
    this->rad = entity_config.entity_size;
    this->color = entity_config.color;
    entity.setPosition({position.x, position.y});
    entity.setRadius(rad);
    entity.setOrigin({rad, rad});
    entity.setFillColor(color);
}