#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <stdexcept>

#include "Utils.hpp"

// Socket設定
#include "SocketHandler.hpp"

// Plyaer状態関連
#include "Player.hpp"
#include "Chaser.hpp"

// グラフィックスUI
#include "GraphicsEngine.hpp"

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

void red_message(std::string msg)
{
    std::cout << "\033[31m" << msg << "\033[0m" << std::endl;
}

int main()
{
    try
    {
        // ゲーム設定
        GameConfig game_config;

        // Udp Socket 初期化
        SocketHandler socket_handler;

        // SFML 初期化
        GraphicsEngine graphics_engine(game_config.window_config);

        // オブジェクト初期化
        static constexpr float playerSize = 10.f, chaserSize = 10.f;
        Player player(game_config.player_config);

        Chaser chaser1(game_config.chaser1_config);
        Chaser chaser2(game_config.chaser2_config);
        Chaser chaser3(game_config.chaser3_config);
        Chaser chaser4(game_config.chaser4_config);



        while (graphics_engine.window.isOpen())
        {
            // SFML evnet
            graphics_engine.UpdateWindowEvent();
            
            // Socket通信
            if (socket_handler.receive())
            {
                sf::Vector2f received_pos = socket_handler.get_received_pos();

                // Player State Update
                player.update(received_pos, graphics_engine.window_size);
            }

            // 敵の追跡アルゴリズム
            chaser1.update(player.getPosition(), graphics_engine.window_size);
            chaser2.update(player.getPosition(), graphics_engine.window_size);
            chaser3.update(player.getEulerPredict(), graphics_engine.window_size);
            chaser4.update(player.getEulerPredict(), graphics_engine.window_size);

            // 描画
            graphics_engine.Render
            (
                player.getEntity(),
                {chaser1.getEntity()
                ,chaser2.getEntity()
                ,chaser3.getEntity()
                ,chaser4.getEntity()});

            // 敵への当たり判定
            for (const auto& chaser : {chaser1, chaser2, chaser3, chaser4})
            {
                if (getDistance(player.getPosition(), chaser.getPosition(), graphics_engine.window_size) < (playerSize + chaserSize))
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