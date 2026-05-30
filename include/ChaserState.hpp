#pragma once
#include <SFML/Graphics.hpp>

class ChaserState
{
public:
    ChaserState(const sf::Vector2f& inipt_pos, float chasing_speed = 5e-5);

    // target_posに向けてspeedで移動
    void update(const sf::Vector2f& target_pos);

    sf::Vector2f get_curr_pos() const {return position;}

private:
    sf::Vector2f position;
    const float speed;
};