#pragma once
#include "Entity.hpp"
#include "SimulationConfig.hpp"

class ChasingObject
{
public:
    ChasingObject(const SimulationConfig::EntityConfig& entity_config);

    void update(const sf::Vector2f& target_pos, const sf::Vector2u& window_size, const float speed);

    sf::Vector2f getPosition() const {return position;}

    const Entity getEntity() const {return entity;}
private:
    Entity entity;
    sf::Vector2f position;
};