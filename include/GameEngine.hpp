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

    GameConfig config;
    SocketHandler socket_handler;
    GraphicsEngine graphics_engine;
    Player player;
    std::vector<Chaser> chasers;
private:
};