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

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Entity.hpp"

void red_message(std::string msg)
{
    std::cout << "\033[31m" << msg << "\033[0m" << std::endl;
}

class ChaserState
{
public:
    ChaserState(sf::Vector2f init_pos, float chasing_speed = 5e-5)
        : position(init_pos)
        , speed(chasing_speed)
    {}

    void update(const sf::Vector2f& target_pos)
    {
        float distance = sqrt(Square(target_pos.x - position.x) + Square(target_pos.y - position.y));
        position.x += (target_pos.x - position.x) / distance * speed;
        position.y += (target_pos.y - position.y) / distance * speed;
    }

    sf::Vector2f get_curr_pos() const {return position;}

private:
    sf::Vector2f position;
    const float speed;
};

class Chaser
{
public:
    Chaser(const sf::Vector2f& init_pos, float rad, const sf::Color& color)
        : state(init_pos)
        , entity(init_pos, rad, color)
    {}

    void update(sf::Vector2f target_pos, sf::Vector2u window_size)
    {
        state.update(target_pos);
        sf::Vector2f curr_pos = state.get_curr_pos();
        entity.entity.setPosition({curr_pos.x * window_size.x, curr_pos.y * window_size.y});
    }

    sf::Vector2f getPosition() const
    {
        return state.get_curr_pos();
    }

    const Entity getEntity() const {return entity;}

private:
    ChaserState state;
    Entity entity;
};

class GraphicsEngine
{
public:
    GraphicsEngine()
        : window(sf::VideoMode(window_size), window_title)
        {}

    // windw
    static constexpr const char* window_title = "Escape from Finger chaser";
    sf::Vector2u window_size = {800, 600};
    sf::RenderWindow window;

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

    void Render(const Player& player, const std::vector<Chaser>& chasers)
    {
        window.clear(sf::Color::Black);
        window.draw(player.getEntity().entity);
        for (auto& chaser : chasers) window.draw(chaser.getEntity().entity);
        window.display();
    }

private:
};

int main()
{
    try
    {
        // Udp Socket 初期化
        SocketHandler socket_handler;
        
        // SFML 初期化
        GraphicsEngine graphics_engine;

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
            graphics_engine.Render(player, {chaser1, chaser2, chaser3, chaser4});

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