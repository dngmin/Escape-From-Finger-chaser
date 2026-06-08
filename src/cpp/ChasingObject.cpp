#include "ChasingObject.hpp"
#include "Utils.hpp"

ChasingObject::ChasingObject(const SimulationConfig::EntityConfig& entity_config)
    : entity(entity_config)
    , position(entity_config.init_pos)
{}

// 座標を0 ~ 1の値でもらうためWindowサイズで合わせる
void ChasingObject::update(const sf::Vector2f& target_pos, const sf::Vector2u& window_size, const float speed)
{
    float distance = getDistance(target_pos, position);
    position.x += (target_pos.x - position.x) / distance * speed;
    position.y += (target_pos.y - position.y) / distance * speed;
    entity.entity.setPosition({position.x * window_size.x, position.y * window_size.y});
}