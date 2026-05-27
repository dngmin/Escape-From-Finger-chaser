#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <stdexcept>

// Socket設定
#include "SocketSetting.hpp"

// Plyaer状態関連
#include "Player.hpp"

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

// struct
struct chaser_State
{
    sf::CircleShape chaser_shape;
    sf::Vector2f position;
};

// function
float float_square(float x)
{
    return x * x;
}

float getDistance_square(const sf::Vector2f& A, const sf::Vector2f& B, const sf::Vector2u& window_size)
{
    return (float_square(window_size.x * (A.x - B.x)) + float_square(window_size.y * (A.y - B.y)));
}

void chasers_initialize(std::vector<chaser_State>& chasers, sf::Vector2u window_size, float rad)
{
    std::vector<sf::Vector2f> init_pos = {{0.1, 0.1}, {0.9, 0.1}, {0.1, 0.9}, {0.9, 0.9}};
    std::vector<std::vector<uint8_t>> init_color = {{255, 0, 0}, {0, 255, 0}, {0, 255, 255}, {255, 255, 0}};
    for (int i = 0; i < 4; i++)
    {
        chasers[i].position = init_pos[i];
        init_pos[i].x *= window_size.x;
        init_pos[i].y *= window_size.y;
        chasers[i].chaser_shape.setRadius(rad);
        chasers[i].chaser_shape.setOrigin({rad, rad});
        chasers[i].chaser_shape.setPosition(init_pos[i]);
        chasers[i].chaser_shape.setFillColor(sf::Color(init_color[i][0], init_color[i][1], init_color[i][2]));
    }
}

void chasing(chaser_State& chaser, const sf::Vector2f& target_pos, sf::Vector2u window_size, float chasing_speed = 5e-5)
{
    float v = sqrt(float_square(target_pos.x - chaser.position.x) + float_square(target_pos.y - chaser.position.y));
    chaser.position.x += (target_pos.x - chaser.position.x)/v * chasing_speed;
    chaser.position.y += (target_pos.y - chaser.position.y)/v * chasing_speed;
    chaser.chaser_shape.setPosition({window_size.x * (chaser.position.x), window_size.y * (chaser.position.y)});
}

void red_message(std::string msg)
{
    std::cout << "\033[31m" << msg << "\033[0m" << std::endl;
}

class GraphicsEngine
{
public:
    GraphicsEngine() :
        window(sf::VideoMode(window_size), window_title),
        player(player_size_rad),
        chasers(4)
        {
            initPlayer();
            initChaser();
        };

    // windw
    static constexpr const char* window_title = "Escape from Finger chaser";
    sf::Vector2u window_size = {800, 600};
    sf::RenderWindow window;

    // Object
    static constexpr float player_size_rad = 10.f;
    static constexpr float chaser_size_rad = 10.f;

    sf::CircleShape player;
    std::vector<chaser_State> chasers;

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

    void Render()
    {
        window.clear(sf::Color::Black);
        window.draw(player);
        for (auto& chaser : chasers) window.draw(chaser.chaser_shape);
        window.display();
    }

private:
    void initPlayer()
    {
        sf::Vector2f init_pos = {0.5, 0.5};
        player.setOrigin({player_size_rad, player_size_rad});
        player.setPosition({init_pos.x * window_size.x, init_pos.y * window_size.y});
    };

    void initChaser()
    {
    std::vector<sf::Vector2f> init_pos = {{0.1, 0.1}, {0.9, 0.1}, {0.1, 0.9}, {0.9, 0.9}};
    std::vector<std::vector<uint8_t>> init_color = {{255, 0, 0}, {0, 255, 0}, {0, 255, 255}, {255, 255, 0}};
    for (int i = 0; i < 4; i++)
    {
        chasers[i].position = init_pos[i];
        init_pos[i].x *= window_size.x;
        init_pos[i].y *= window_size.y;
        chasers[i].chaser_shape.setRadius(chaser_size_rad);
        chasers[i].chaser_shape.setOrigin({chaser_size_rad, chaser_size_rad});
        chasers[i].chaser_shape.setPosition(init_pos[i]);
        chasers[i].chaser_shape.setFillColor(sf::Color(init_color[i][0], init_color[i][1], init_color[i][2]));
    }
    };
};

int main()
{
    try
    {
        // Udp Socket 初期化
        SocketSetting socket_setting;
        
        // SFML 初期化
        GraphicsEngine graphics_engine;

        // player position 初期化
        PlayerState player_state;

        while (graphics_engine.window.isOpen())
        {
            // SFML evnet
            graphics_engine.UpdateWindowEvent();
            
            // Socket通信
            if (socket_setting.receive())
            {
                sf::Vector2f received_pos = socket_setting.get_received_pos();

                // Player State Update
                player_state.update(received_pos);

                // set player
                sf::Vector2f curr_pos = player_state.get_curr_pos();
                graphics_engine.player.setPosition({curr_pos.x * graphics_engine.window_size.x, curr_pos.y * graphics_engine.window_size.y});
            }

            // 敵の追跡アルゴリズム
            for (int i = 0; i < 4; i++)
            {
                if (i < 2) chasing(graphics_engine.chasers[i], player_state.get_curr_pos(), graphics_engine.window_size);
                else chasing(graphics_engine.chasers[i], player_state.get_Euler_predict(), graphics_engine.window_size);
            }

            // 描画
            graphics_engine.Render();

            // 敵への当たり判定
            for (const auto& chaser : graphics_engine.chasers)
            {
                if (getDistance_square(player_state.get_curr_pos(), chaser.position, graphics_engine.window_size) < ((graphics_engine.player_size_rad + graphics_engine.chaser_size_rad) * (graphics_engine.player_size_rad + graphics_engine.chaser_size_rad)))
                {
                    return 0;
                }
            }

        }
    }
    catch (const envError& e)
    {
        std::cout << e.what() << std::endl;
        
        switch (e.getError())
        {
            case envError::Error::FILE_NOT_FOUND:
                red_message("→ 解決方法: .envファイルがあるか又はPathを確認してください"); break;
            case envError::Error::PORT_NUMBER_NOT_NONNEGATIVE_INTEGER:
                red_message("→ 解決方法: .envファイルのport_numerを確認してください"); break;
            case envError::Error::PORT_NUMBER_NOT_FOUND:
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