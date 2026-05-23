#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <fstream>
#include <stdexcept>

// Plyaer状態関連
#include "Player.hpp"

// .env読み込み
#include "env_load.hpp"

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

int main()
{
    try
    {
    int port_number = get_port_number();
    }
    catch (const envError& e)
    {
        std::cout << e.what() << std::endl;
        
        switch (e.getType())
        {
            case envError::Type::FILE_NOT_FOUND:
                red_message("→ 解決方法: .envファイルがあるか又はPathを確認してください"); break;
            case envError::Type::PORT_NUMBER_NOT_NONNEGATIVE_INTEGER:
                red_message("→ 解決方法: .envファイルのport_numerを確認してください"); break;
            case envError::Type::PORT_NUMBER_NOT_FOUND:
                red_message("→ 解決方法: .envファイルにport_numberがあるか確認してください"); break;
        }
    }

    // Udp Socket 初期化
    sf::UdpSocket socket;
    std::size_t received;
    std::optional<sf::IpAddress> sender;
    unsigned short port;
    socket.setBlocking(false);
    if (socket.bind(5005) != sf::Socket::Status::Done) return -1;

    // SFML 初期化
    sf::Vector2u window_size = {800, 600};
    sf::RenderWindow window(sf::VideoMode(window_size), "display");

    float player_size_rad = 10.f;
    float chaser_size_rad = 10.f;

    // player
    sf::CircleShape player(player_size_rad);
    player.setOrigin({player_size_rad, player_size_rad});
    // 敵
    std::vector<chaser_State> chasers(4);
    chasers_initialize(chasers, window_size, chaser_size_rad);

    // player position 初期化
    PlayerState player_state;
    sf::Vector2f received_pos;

    {
        sf::Vector2f curr_pos = player_state.get_curr_pos();
        player.setPosition({curr_pos.x * window_size.x, curr_pos.y * window_size.y});
    }

    while (window.isOpen())
    {
        // SFML evnet
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
        
        // Socket通信
        if (socket.receive(&received_pos, sizeof(received_pos), received, sender, port) == sf::Socket::Status::Done)
        {
            // Player State Update
            player_state.update(received_pos);

            // set player
            sf::Vector2f curr_pos = player_state.get_curr_pos();
            player.setPosition({curr_pos.x * window_size.x, curr_pos.y * window_size.y});
        }

        // 敵の追跡アルゴリズム
        
        // 単純追跡
        // for (auto& chaser : chasers) chasing(chaser, player_state.get_curr_pos(), window_size);
        for (int i = 0; i < 4; i++)
        {
            if (i < 2) chasing(chasers[i], player_state.get_curr_pos(), window_size);
            else chasing(chasers[i], player_state.get_Euler_predict(), window_size);
        }

        // 描画
        window.clear(sf::Color::Black);
        window.draw(player);
        for (auto& chaser : chasers) window.draw(chaser.chaser_shape);

        window.display();

        // 敵への当たり判定
        for (const auto& chaser : chasers)
        {
            if (getDistance_square(player_state.get_curr_pos(), chaser.position, window_size) < ((player_size_rad + chaser_size_rad) * (player_size_rad + chaser_size_rad)))
            {
                return 0;
            }
        }

    }









    return 0;
}