#pragma once
#include <SFML/Graphics.hpp>

class Entity
{
public:
    Entity(const sf::Vector2f& init_pos, float rad, const sf::Color& color);
    sf::CircleShape entity;
    sf::Vector2f position;
private:
    float rad;
    sf::Color color;
};