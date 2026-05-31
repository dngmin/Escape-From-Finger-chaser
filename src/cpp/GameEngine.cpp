#include "GameEngine.hpp"

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