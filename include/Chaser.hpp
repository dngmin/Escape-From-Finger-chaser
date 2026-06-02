#pragma once
#include "ChaserState.hpp"
#include "Entity.hpp"
#include "GameConfig.hpp"

class Chaser
{
public:
    Chaser(const GameConfig::EntityConfig& entity_config);

    void update(const sf::Vector2f& target_pos, const sf::Vector2u& window_size, const float speed);

    sf::Vector2f getPosition() const;

    const Entity getEntity() const {return entity;}
private:
    ChaserState state;
    Entity entity;
};