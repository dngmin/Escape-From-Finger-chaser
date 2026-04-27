#include <iostream>
#include <vector>

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

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
    sf::Vector2f finger_position = {-100.f, -100.f};


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
        if (socket.receive(&finger_position, sizeof(finger_position), received, sender, port) == sf::Socket::Status::Done)
        {
            finger_position.x *= window_size.x;
            finger_position.y *= window_size.y;
        }



        


        // 敵への当たり判定
        for (const auto& chaser : chasers)
        {
            if (getDistance_square(player, chaser) < 4*rad*rad)
            {
                std::cout << "BoB";
                return 0;
            }
        }


        
        // 描画
        window.clear(sf::Color::Black);
        player.setPosition({finger_position.x, finger_position.y});
        window.draw(player);
        for (auto& chaser : chasers) window.draw(chaser);

        window.display();
    }









    return 0;
}