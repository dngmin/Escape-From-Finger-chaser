#include "GameEngine.hpp"
#include "Utils.hpp"

GameEngine::GameEngine(const GameConfig& game_config)
    : config(game_config)
    , graphics_engine(game_config.window_config)
    , player(game_config.player_config)
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
        graphics_engine.UpdateWindowEvent();
        UpdatePlayerPosition();
        Chasing();
        Render();
        if (CollisionDetection()) return false;
    }
    return true;
}

void GameEngine::UpdatePlayerPosition()
{
    if (socket_handler.receive())
    {
        sf::Vector2f received_pos = socket_handler.get_received_pos();
        player.update(received_pos, graphics_engine.window_size);
    }
}

void GameEngine::WindowEvent()
{
    graphics_engine.UpdateWindowEvent();
}

void GameEngine::Chasing()
{
    // Chaser1,2：単純追跡
    // Chaser3,4：オイラー法を用いた予測追跡
    // 今後二つのアルゴリズムを追加し、4種の追跡アルゴリズム適用予定
    for (int i = 0; i < config.chasers_config.chaserCount; i++)
    {
        if (i < 2) chasers[i].update(player.getPosition(), graphics_engine.window_size);
        else chasers[i].update(player.getEulerPredict(), graphics_engine.window_size);
    }
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