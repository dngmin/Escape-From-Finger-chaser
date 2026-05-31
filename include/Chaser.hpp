#pragma once
#include "ChaserState.hpp"
#include "Entity.hpp"
#include "GameConfig.hpp"

class Chaser
{
public:
    Chaser(const GameConfig::EntityConfig& entity_config);

    void update(sf::Vector2f target_pos, sf::Vector2u window_size);

    sf::Vector2f getPosition() const;

    const Entity getEntity() const {return entity;}
private:
    ChaserState state;
    Entity entity;
};