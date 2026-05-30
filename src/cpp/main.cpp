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
        // Udp Socket 初期化
        SocketHandler socket_handler;
        
        // SFML 初期化
        // タイトル、サイズ、背景色
        GraphicsEngine graphics_engine("game", {800, 600}, sf::Color::Black);

        // オブジェクト初期化
        // 初期位置、Entityサイズ、色
        static constexpr float playerSize = 10.f, chaserSize = 10.f;
        Player player({0.5f, 0.5f}, playerSize, {255, 255, 255});

        // std::vector<sf::Vector2f> chasers_init_pos = {{0.1, 0.1}, {0.9, 0.1}, {0.1, 0.9}, {0.9, 0.9}};
        // std::vector<sf::Color> chasers_color = {{255, 0, 0}, {0, 255, 0}, {0, 255, 255}, {255, 255, 0}};
        Chaser chaser1({0.1, 0.1}, chaserSize, {255, 0, 0});
        Chaser chaser2({0.9, 0.1}, chaserSize, {255, 0, 0});
        Chaser chaser3({0.1, 0.9}, chaserSize, {0, 255, 0});
        Chaser chaser4({0.9, 0.9}, chaserSize, {0, 255, 0});



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