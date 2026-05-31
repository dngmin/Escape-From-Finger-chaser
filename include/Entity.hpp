#pragma once
#include <SFML/Graphics.hpp>
#include "GameConfig.hpp"

class Entity
{
public:
    Entity(const GameConfig::EntityConfig entity_config);
    sf::CircleShape entity;
    sf::Vector2f position;
private:
    float rad;
    sf::Color color;
};