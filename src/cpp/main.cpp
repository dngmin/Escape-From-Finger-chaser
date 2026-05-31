#include <iostream>
#include <vector>
#include <string>

#include "Utils.hpp"

// Socket設定
#include "SocketHandler.hpp"

// Plyaer状態関連
#include "Player.hpp"
#include "Chaser.hpp"

// グラフィックスUI
#include "GraphicsEngine.hpp"

void red_message(std::string msg)
{
    std::cout << "\033[31m" << msg << "\033[0m" << std::endl;
}

class GameEngine
{
public:
    GameEngine(const GameConfig& config)
        : graphics_engine(config.window_config)
        , player(config.player_config)
    {
        // 初回描画のために、正規化された座標の変換処理を1回だけ行う
        player.update(config.player_config.init_pos, config.window_config.window_size);

        chasers.reserve(config.chasers_config.chaserCount);
        for (auto& chaser_config : config.chasers_config.chasers)
        {
            chasers.emplace_back(chaser_config);
        }
    }

    SocketHandler socket_handler;
    GraphicsEngine graphics_engine;
    Player player;
    std::vector<Chaser> chasers;

private:
};

int main()
{
    try
    {
        // ゲーム設定
        GameConfig game_config;
        GameEngine game_engine(game_config);

        while (game_engine.graphics_engine.window.isOpen())
        {
            // SFML evnet
            game_engine.graphics_engine.UpdateWindowEvent();
            
            // Socket通信
            if (game_engine.socket_handler.receive())
            {
                sf::Vector2f received_pos = game_engine.socket_handler.get_received_pos();

                // Player State Update
                game_engine.player.update(received_pos, game_engine.graphics_engine.window_size);
            }

            // 敵の追跡アルゴリズム
            for (int i = 0; i < game_config.chasers_config.chaserCount; i++)
            {
                if (i < 2) game_engine.chasers[i].update(game_engine.player.getPosition(), game_engine.graphics_engine.window_size);
                else game_engine.chasers[i].update(game_engine.player.getEulerPredict(), game_engine.graphics_engine.window_size);
            }

            // 描画
            game_engine.graphics_engine.Render
            (
                game_engine.player.getEntity(),
                {game_engine.chasers[0].getEntity()
                ,game_engine.chasers[1].getEntity()
                ,game_engine.chasers[2].getEntity()
                ,game_engine.chasers[3].getEntity()});

            // 敵への当たり判定
            for (int i = 0; i < game_config.chasers_config.chaserCount; i++)
            {
                if (getDistance(game_engine.player.getPosition(), game_engine.chasers[i].getPosition(), game_engine.graphics_engine.window_size) < (game_config.player_config.entity_size + game_config.chasers_config.chasers[i].entity_size))
                {
                    return 0;
                }
            }

        }
    }
    catch (const ConfigError& e)
    {
        std::cout << e.what() << std::endl;
        
        switch (e.getError())
        {
            case ConfigError::Error::FILE_NOT_FOUND:
                red_message("→ 解決方法: .envファイルがあるか又はPathを確認してください"); break;
            case ConfigError::Error::PORT_NUMBER_NOT_NONNEGATIVE_INTEGER:
                red_message("→ 解決方法: .envファイルのport_numerを確認してください"); break;
            case ConfigError::Error::PORT_NUMBER_NOT_FOUND:
                red_message("→ 解決方法: .envファイルにport_numberがあるか確認してください"); break;
        }
    }
    catch (const SocketError& e)
    {
        std::cout << e.what() << std::endl;

        switch (e.getError())
        {
            case SocketError::Error::FAILED_BINDING:
                red_message("→ 解決方法: 以下の三つの原因を確認してください\n1. port numberが既に使用中である\n2. 権限問題\n3. 不正なポートナンバー"); break;
            case SocketError::Error::SOCKET_NOT_VALID:
                red_message("→ 解決方法: ネットワークの接続を確認し、プログラムを再起動してください"); break;
        }
        
    }








    return 0;
}