#pragma once
// #include "PlayerState.hpp"
#include "Entity.hpp"
#include "GameConfig.hpp"
#include "OneEuroFilter.hpp"
#include <chrono>

class Player
{
    // timestampに使用するchronoを短縮定義
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

public:
    Player(const GameConfig::EntityConfig& entity_config);

    // 1€ filterを用いたPlayerのデータアップデート
    void update(const sf::Vector2f& received_pos, const sf::Vector2u& window_size);

    sf::Vector2f getPosition() const {return curr_pos;}

    sf::Vector2f getVelocity() const {return curr_vel;}

    float getdT() const {return dT;}

    const Entity getEntity() const {return entity;}
private:
    Entity entity;
    OneEuroFilter filter;
    // Playerのデータ
    sf::Vector2f curr_pos; // 現在位置
    sf::Vector2f curr_vel = {0.f, 0.f}; // 現在速度

    // 基準とする時間(初回呼び出し時に初期化され、update関数より更新される)
    TimePoint T_prev;
    float dT;
};