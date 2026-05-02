#include <iostream>
#include <vector>

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

// struct
struct player_State
{
    sf::Vector2f curr_pos;
    sf::Vector2f prev_pos = {-100.f, -100.f};
    sf::Vector2f curr_vel;
    sf::Vector2f prev_vel;
    sf::Vector2f curr_acc;
    sf::Vector2f prev_acc;
};

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

void chasing(chaser_State& chaser, const sf::Vector2f& target_pos, sf::Vector2u window_size, float chasing_speed = 2e-5)
{
    float v = sqrt(pow(target_pos.x - chaser.position.x,2) + pow(target_pos.y - chaser.position.y,2));
    chaser.position.x += (target_pos.x - chaser.position.x)/v * chasing_speed;
    chaser.position.y += (target_pos.y - chaser.position.y)/v * chasing_speed;
    chaser.chaser_shape.setPosition({window_size.x * (chaser.position.x), window_size.y * (chaser.position.y)});
}

sf::Vector2f predict_Euler(player_State& state)
{
    // x_t+1 = xt + vt*t + 0.5*a*t^2
    return {0.f, 0.f};
}

// constant
const int socket_port_num = 5005;

int main()
{
    // Udp Socket 初期化
    sf::UdpSocket socket;
    std::size_t received;
    std::optional<sf::IpAddress> sender;
    unsigned short port;
    socket.setBlocking(false);
    if (socket.bind(socket_port_num) != sf::Socket::Status::Done) return -1;


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
    player_State player_state;


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
        if (socket.receive(&player_state.curr_pos, sizeof(player_state.curr_pos), received, sender, port) == sf::Socket::Status::Done)
        {
            player.setPosition({player_state.curr_pos.x * window_size.x, player_state.curr_pos.y * window_size.y});
        }

        // 敵の追跡アルゴリズム
        
        // 単純追跡
        for (auto& chaser : chasers) chasing(chaser, player_state.curr_pos, window_size);


        


        // 敵への当たり判定
        for (const auto& chaser : chasers)
        {
            if (getDistance_square(player_state.curr_pos, chaser.position, window_size) < ((player_size_rad + chaser_size_rad) * (player_size_rad + chaser_size_rad)))
            {
                return 0;
            }
        }


        
        // 描画
        window.clear(sf::Color::Black);
        window.draw(player);
        for (auto& chaser : chasers) window.draw(chaser.chaser_shape);

        window.display();
    }









    return 0;
}