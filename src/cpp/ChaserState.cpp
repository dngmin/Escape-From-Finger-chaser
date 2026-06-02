#include "ChaserState.hpp"
#include "Utils.hpp"
#include <cassert>

ChaserState::ChaserState(const sf::Vector2f& init_pos)
    : position(init_pos)
{}

// ベクトル分解を用いて target_posへの１フレーム間移動を計算
void ChaserState::update(const sf::Vector2f& target_pos, const float speed)
{
    float distance = getDistance(target_pos, position);
    position.x += (target_pos.x - position.x) / distance * speed;
    position.y += (target_pos.y - position.y) / distance * speed;
}