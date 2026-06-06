#include "GameEngine.hpp"
#include "Utils.hpp"
#include "ChasingAlgorithm.hpp"

GameEngine::GameEngine(const GameConfig& game_config)
    : config(game_config)
    , graphics_engine(game_config.window_config)
    , player(game_config.player_config)
    , prediction_steps(game_config.chasers_config.prediction_steps)
    , chaser_speed(game_config.chasers_config.speed)
    {
        // 初回描画のために、正規化された座標の変換処理を1回だけ行う
        player.update(config.player_config.init_pos, config.window_config.window_size);

        chasers.reserve(config.chasers_config.chaserCount);
        for (auto& chaser_config : config.chasers_config.chasers)
        {
            chasers.emplace_back(chaser_config);
        }
    }

bool GameEngine::run()
{
    while (graphics_engine.window.isOpen())
    {
        WindowEvent();
        UpdatePlayerPosition();
        Chasing();
        Render();
        if (CollisionDetection()) return false;
    }
    return true;
}

void GameEngine::WindowEvent()
{
    graphics_engine.UpdateWindowEvent();
}

void GameEngine::UpdatePlayerPosition()
{
    if (socket_handler.receive())
    {
        sf::Vector2f received_pos = socket_handler.get_received_pos();
        player.update(received_pos, graphics_engine.window_size);
    }
}

void GameEngine::Chasing()
{
    sf::Vector2f player_curr_pos = player.getPosition();
    sf::Vector2f player_curr_vel = player.getVelocity();
    float dT = player.getdT();
    sf::Vector2f player_Eulerpredict_pos = predictEulerPosition(player_curr_pos, player_curr_vel, dT, prediction_steps);

    // Chaser1：単純追跡
    // Chaser2,3：オイラー法を用いた予測追跡
    // Chaser4：ランダム座標に向けた追跡
    // 後もう一つのアルゴリズムを追加し、4種の追跡アルゴリズム適用予定
    chasers[0].update(player_curr_pos, graphics_engine.window_size, chaser_speed);
    chasers[1].update(player_Eulerpredict_pos, graphics_engine.window_size, chaser_speed);
    chasers[2].update(player_Eulerpredict_pos, graphics_engine.window_size, chaser_speed);
    sf::Vector2f random_pos = getRandomPosition(player_curr_pos, chasers[3].getPosition());
    chasers[3].update(random_pos, graphics_engine.window_size, chaser_speed);
}

void GameEngine::Render()
{
    graphics_engine.Render
    (
        player.getEntity(),
        {chasers[0].getEntity()
        ,chasers[1].getEntity()
        ,chasers[2].getEntity()
        ,chasers[3].getEntity()}
    );
}

bool GameEngine::CollisionDetection()
{
    for (int i = 0; i < config.chasers_config.chaserCount; i++)
    {
        float distance_player_to_chaser = getDistance(player.getPosition(), chasers[i].getPosition(), graphics_engine.window_size);
        float collision_threshold = config.player_config.entity_size + config.chasers_config.chasers[i].entity_size;
        if (distance_player_to_chaser <= collision_threshold) return true;
    }
    return false;
}