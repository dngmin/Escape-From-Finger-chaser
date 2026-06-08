#pragma once
#include "SimulationConfig.hpp"
#include "SocketHandler.hpp"
#include "GraphicsEngine.hpp"
#include "Player.hpp"
#include "ChasingObject.hpp"

class SimulationEngine
{
public:
    SimulationEngine(const SimulationConfig& game_config);

    bool run();

    void WindowEvent();
    
    void UpdatePlayerPosition();

    void Chasing();

    void Render();

    bool CollisionDetection();

private:
    SimulationConfig config;
    SocketHandler socket_handler;
    GraphicsEngine graphics_engine;
    Player player;
    std::vector<ChasingObject> chasing_objects;
    int prediction_steps;
    float chasing_object_speed;
    float distanceThreshold = 1.f;
};