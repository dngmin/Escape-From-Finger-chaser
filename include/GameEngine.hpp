#pragma once
#include "GameConfig.hpp"
#include "SocketHandler.hpp"
#include "GraphicsEngine.hpp"
#include "Player.hpp"
#include "Chaser.hpp"

class GameEngine
{
public:
    GameEngine(const GameConfig& game_config);

    bool run();

    void WindowEvent();
    
    void UpdatePlayerPosition();

    void Chasing();

    void Render();

    bool CollisionDetection();

private:
    GameConfig config;
    SocketHandler socket_handler;
    GraphicsEngine graphics_engine;
    Player player;
    std::vector<Chaser> chasers;
    int prediction_steps;
    float chaser_speed;
};