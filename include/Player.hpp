#pragma once
#include "PlayerState.hpp"
#include "Entity.hpp"

class Player
{
public:
    Player(const sf::Vector2f& init_pos, float rad, const sf::Color& color);

    void update(const sf::Vector2f& received_pos, const sf::Vector2u& window_size);

    sf::Vector2f getPosition() const;

    sf::Vector2f getEulerPredict() const;

    const Entity getEntity() const {return entity;}
private:
    PlayerState state;
    Entity entity;
};