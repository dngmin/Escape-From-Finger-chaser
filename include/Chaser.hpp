#pragma once
#include "ChaserState.hpp"
#include "Entity.hpp"

class Chaser
{
public:
    Chaser(const sf::Vector2f& init_pos, float rad, const sf::Color& color);

    void update(sf::Vector2f target_pos, sf::Vector2u window_size);

    sf::Vector2f getPosition() const;

    const Entity getEntity() const {return entity;}
private:
    ChaserState state;
    Entity entity;
};