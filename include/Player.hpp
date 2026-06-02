#pragma once
#include "PlayerState.hpp"
#include "Entity.hpp"
#include "GameConfig.hpp"

class Player
{
public:
    Player(const GameConfig::EntityConfig& entity_config);

    void update(const sf::Vector2f& received_pos, const sf::Vector2u& window_size);

    sf::Vector2f getPosition() const;

    sf::Vector2f getprevPosition() const;

    sf::Vector2f getVelocity() const;

    float getdT() const;

    const Entity getEntity() const {return entity;}
private:
    PlayerState state;
    Entity entity;
};