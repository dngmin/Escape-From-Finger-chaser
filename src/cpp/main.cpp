#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <stdexcept>

// Socket設定
#include "SocketHandler.hpp"

// Plyaer状態関連
#include "Player.hpp"

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Entity.hpp"

// function
float float_square(float x)
{
    return x * x;
}

float getDistance_square(const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2u& window_size)
{
    return (float_square(window_size.x * (A.x - B.x)) + float_square(window_size.y * (A.y - B.y)));
}

void red_message(std::string msg)
{
    std::cout << "\033[31m" << msg << "\033[0m" << std::endl;
}

void chasing(Entity& chaser, const sf::Vector2f& target_pos, sf::Vector2u window_size, float chasing_speed = 5e-5)
{
    float v = sqrt(float_square(target_pos.x - chaser.position.x) + float_square(target_pos.y - chaser.position.y));
    chaser.position.x += (target_pos.x - chaser.position.x)/v * chasing_speed;
    chaser.position.y += (target_pos.y - chaser.position.y)/v * chasing_speed;
    chaser.entity.setPosition({window_size.x * (chaser.position.x), window_size.y * (chaser.position.y)});
}

class GraphicsEngine
{
public:
    GraphicsEngine()
        : window(sf::VideoMode(window_size), window_title)
        {
            SetupObject();
        }

    // windw
    static constexpr const char* window_title = "Escape from Finger chaser";
    sf::Vector2u window_size = {800, 600};
    sf::RenderWindow window;

    // Entities
    std::vector<Entity> chasers;
    static constexpr int chaserCount = 4;
    static constexpr float player_size_rad = 10.f;
    static constexpr float chaser_size_rad = 10.f;

    void UpdateWindowEvent()
    {
        while (std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) window.close();

            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                window_size = window.getSize();
                sf::View view(sf::FloatRect({0.f, 0.f}, sf::Vector2f(window_size)));
                window.setView(view);
            }
        }
    }

    void Render(const Entity& player)
    {
        window.clear(sf::Color::Black);
        window.draw(player.entity);
        for (auto& chaser : chasers) window.draw(chaser.entity);
        window.display();
    }

private:
    void SetupObject()
    {
        // Chaser components
        std::vector<sf::Vector2f> chasers_init_pos = {{0.1, 0.1}, {0.9, 0.1}, {0.1, 0.9}, {0.9, 0.9}};
        std::vector<sf::Color> chasers_color = {{255, 0, 0}, {0, 255, 0}, {0, 255, 255}, {255, 255, 0}};
        for (int i = 0; i < chaserCount; i++)
        {
            chasers.emplace_back(
                chasers_init_pos[i],
                chaser_size_rad,
                chasers_color[i]
            );
        }
    }
};

int main()
{
    try
    {
        // Udp Socket 初期化
        SocketHandler socket_handler;
        
        // SFML 初期化
        GraphicsEngine graphics_engine;

        // player position 初期化
        // 初期位置、Entityサイズ、色
        Player player({0.5f, 0.5f}, 10.f, {255, 255, 255});

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
            for (int i = 0; i < graphics_engine.chaserCount; i++)
            {
                if (i < 2) chasing(graphics_engine.chasers[i], player.getPosition(), graphics_engine.window_size);
                else chasing(graphics_engine.chasers[i], player.getEulerPredict(), graphics_engine.window_size);
            }

            // 描画
            graphics_engine.Render(player.getEntity());

            // 敵への当たり判定
            for (const auto& chaser : graphics_engine.chasers)
            {
                if (getDistance_square(player.getPosition(), chaser.position, graphics_engine.window_size) < ((graphics_engine.player_size_rad + graphics_engine.chaser_size_rad) * (graphics_engine.player_size_rad + graphics_engine.chaser_size_rad)))
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