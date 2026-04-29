#include <iostream>
#include <vector>

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

// struct
struct State
{
    sf::Vector2f curr_pos;
    sf::Vector2f prev_pos;
    sf::Vector2f curr_vel;
    sf::Vector2f prev_vel;
    sf::Vector2f curr_acc;
    sf::Vector2f prev_acc;
};

// function
float float_square(float x)
{
    return x * x;
}

float getDistance_square(const sf::CircleShape& player, const sf::CircleShape& chaser)
{
    return (float_square(player.getPosition().x - chaser.getPosition().x) + float_square(player.getPosition().y - chaser.getPosition().y));
}

void chasers_initialize(std::vector<sf::CircleShape>& chasers, sf::Vector2u window_size, float rad)
{
    std::vector<sf::Vector2f> init_pos = {{0.1, 0.1}, {0.9, 0.1}, {0.1, 0.9}, {0.9, 0.9}};
    std::vector<std::vector<u_int8_t>> init_color = {{255, 0, 0}, {0, 255, 0}, {0, 255, 255}, {255, 255, 0}};
    int i = 0;
    for (int i = 0; i < 4; i++)
    {
        init_pos[i].x *= window_size.x;
        init_pos[i].y *= window_size.y;
        chasers[i].setRadius(rad);
        chasers[i].setOrigin({rad, rad});
        chasers[i].setPosition(init_pos[i]);
        chasers[i].setFillColor(sf::Color(init_color[i][0], init_color[i][1], init_color[i][2]));
    }
}

void chasing(sf::CircleShape& chaser, const sf::Vector2f& target_pos)
{
    float chasing_speed = 2e-2;
    sf::Vector2f chaser_pos = chaser.getPosition();
    float v = sqrt(pow(target_pos.x - chaser_pos.x,2) + pow(target_pos.y - chaser_pos.y,2));
    float vx = (target_pos.x - chaser_pos.x)/v * chasing_speed;
    float vy = (target_pos.y - chaser_pos.y)/v * chasing_speed;
    chaser.setPosition({chaser_pos.x + vx, chaser_pos.y + vy});
}

sf::Vector2f chasing(sf::Vector2f chaser_pos, sf::Vector2f target_pos)
{
    float chasing_speed = 2e-2;
    float v = sqrt(pow(target_pos.x - chaser_pos.x,2) + pow(target_pos.y - chaser_pos.y,2));
    float vx = (target_pos.x - chaser_pos.x)/v * chasing_speed;
    float vy = (target_pos.y - chaser_pos.y)/v * chasing_speed;
    return {vx, vy};
}

sf::Vector2f predict_Euler(State& state)
{
    // x_t+1 = xt + vt*t + 0.5*a*t^2
}

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

    float rad = 10.f;

    // player
    sf::CircleShape player(rad);
    player.setOrigin({rad, rad});
    // 敵
    std::vector<sf::CircleShape> chasers(4);
    chasers_initialize(chasers, window_size, rad);


    // finger_posiion 初期化
    State finger;
    finger.prev_pos = {-100.f, -100.f};


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
        if (socket.receive(&finger.curr_pos, sizeof(finger.curr_pos), received, sender, port) == sf::Socket::Status::Done)
        {
            finger.curr_pos.x *= window_size.x;
            finger.curr_pos.y *= window_size.y;
        }

        // 敵の追跡アルゴリズム
        
        // 単純追跡
        for (auto& chaser : chasers) chasing(chaser, finger.curr_pos);


        


        // 敵への当たり判定
        for (const auto& chaser : chasers)
        {
            if (getDistance_square(player, chaser) < 4*rad*rad)
            {
                return 0;
            }
        }


        
        // 描画
        window.clear(sf::Color::Black);
        player.setPosition({finger.curr_pos.x, finger.curr_pos.y});
        window.draw(player);
        for (auto& chaser : chasers) window.draw(chaser);

        window.display();
    }









    return 0;
}