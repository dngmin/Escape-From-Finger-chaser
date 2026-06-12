#include "SimulationEngine.hpp"
#include "Utils.hpp"
#include "ChasingAlgorithm.hpp"

SimulationEngine::SimulationEngine(const SimulationConfig& simulation_config)
    : config(simulation_config)
    , graphics_engine(simulation_config.window_config)
    , player(simulation_config.player_config)
    , prediction_steps(simulation_config.chasing_objects_config.prediction_steps)
    , chasing_object_speed(simulation_config.chasing_objects_config.speed)
    {
        // 初回描画のために、正規化された座標の変換処理を1回だけ行う
        player.update(config.player_config.init_pos, config.window_config.window_size);

        chasing_objects.reserve(config.chasing_objects_config.ChasingObjectCount);
        for (auto& chasing_object_config : config.chasing_objects_config.chasing_objects)
        {
            chasing_objects.emplace_back(chasing_object_config);
        }
    }

bool SimulationEngine::run()
{
    while (graphics_engine.window.isOpen())
    {
        WindowEvent();
        UpdatePlayerPosition();
        Chasing();
        Render();
        if (CollisionDetection()) AwaitWindowClose();
    }
    return false;
}

void SimulationEngine::WindowEvent()
{
    graphics_engine.UpdateWindowEvent(stage);
}

void SimulationEngine::UpdatePlayerPosition()
{
    if (socket_handler.receive())
    {
        sf::Vector2f received_pos = socket_handler.get_received_pos();
        player.update(received_pos, graphics_engine.window_size);
    }
}

void SimulationEngine::Chasing()
{
    sf::Vector2f player_curr_pos = player.getPosition();
    sf::Vector2f player_curr_vel = player.getVelocity();
    float dT = player.getdT();
    sf::Vector2f player_Eulerpredict_pos = predictEulerPosition(player_curr_pos, player_curr_vel, dT, prediction_steps);

    // chasing_object1：単純追跡
    // chasing_object2：オイラー法を用いた予測追跡
    // chasing_object4: 軸移動追跡
    // chasing_object4：ランダム座標に向けた追跡
    chasing_objects[0].update(player_curr_pos, graphics_engine.window_size, chasing_object_speed);
    chasing_objects[1].update(player_Eulerpredict_pos, graphics_engine.window_size, chasing_object_speed);

    sf::Vector2f AxisAligned_pos = getAxisAlignedPosition(player_curr_pos, player_curr_vel, chasing_objects[2].getPosition(), distanceThreshold, graphics_engine.window_size);
    chasing_objects[2].update(AxisAligned_pos, graphics_engine.window_size, chasing_object_speed);
    
    static sf::Vector2f random_pos = getRandomPosition(player_curr_pos, chasing_objects[3].getPosition());
    float distance_to_target = getDistance(chasing_objects[3].getPosition(), random_pos, graphics_engine.window_size);
    if (distance_to_target < distanceThreshold)
    {
        random_pos = getRandomPosition(player_curr_pos, chasing_objects[3].getPosition());
    }
    chasing_objects[3].update(random_pos, graphics_engine.window_size, chasing_object_speed);
}

void SimulationEngine::Render()
{
    graphics_engine.Render
    (
        player.getEntity(),
        {chasing_objects[0].getEntity()
        ,chasing_objects[1].getEntity()
        ,chasing_objects[2].getEntity()
        ,chasing_objects[3].getEntity()}
        , stage
    );
}

bool SimulationEngine::CollisionDetection()
{
    for (int i = 0; i < config.chasing_objects_config.ChasingObjectCount; i++)
    {
        float distance_player_to_chasing_object = getDistance(player.getPosition(), chasing_objects[i].getPosition(), graphics_engine.window_size);
        float collision_threshold = config.player_config.entity_size + config.chasing_objects_config.chasing_objects[i].entity_size;
        if (distance_player_to_chasing_object <= collision_threshold)
        {
            stage = Stage::CLOSISING;
            return true;
        }
    }
    return false;
}

void SimulationEngine::AwaitWindowClose()
{
    while (graphics_engine.window.isOpen())
    {
        WindowEvent();
        Render();
    }
}