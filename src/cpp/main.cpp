#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

// constant
const int socket_port_num = 5005;

// class
class playerState
{
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    private:
    sf::Vector2f curr_pos;
    sf::Vector2f prev_pos;
    sf::Vector2f curr_vel;
    sf::Vector2f prev_vel;
    sf::Vector2f curr_acc;

    // 1€ filter 初期値
    TimePoint T_prev = Clock::now();
    double dT;
    static constexpr double min_cutoff_frequency = 1.0; // f_c_min：最小遮断周波数
    static constexpr double beta = 0.01;
    static constexpr double derivative_cutoff = 1.0; // d_cutoff；微分値の遮断周波数
    static constexpr double two_pi = 6.2831853071795865;

    double get_alpha(double dT, double cutoff)
    {
        return 1 / (1 + 1 / (two_pi * dT * cutoff));
    }

    double solve_low_pass_filter(double curr, double prev, double alpha)
    {
        return alpha * curr + (1 - alpha) * prev;
    }

    public:
    sf::Vector2f get_curr_pos()
    {
        return curr_pos;
    }

    sf::Vector2f get_Euler_predict()
    {
        // x_t+1 = xt + vt*dt
        float predict_x = curr_pos.x + curr_vel.x * (5 * dT);
        float predict_y = curr_pos.y + curr_vel.y * (5 * dT);
        return {predict_x, predict_y};
    }

    void update(sf::Vector2f received_pos)
    {
        if (received_pos == curr_pos) return;

        // 1€ filter
        // calculate velocity
        TimePoint T = Clock::now();
        dT = std::chrono::duration<double>(T - T_prev).count();
        curr_vel.x = (received_pos.x - prev_pos.x) / dT;
        curr_vel.y = (received_pos.y - prev_pos.y) / dT;

        // filtering velocity
        double alpha_derivative = get_alpha(dT, derivative_cutoff);
        curr_vel.x = solve_low_pass_filter(curr_vel.x, prev_vel.x, alpha_derivative);
        curr_vel.y = solve_low_pass_filter(curr_vel.y, prev_vel.y, alpha_derivative);

        // adapted alpha
        double cutoff_frequency_x = min_cutoff_frequency + beta * abs(curr_vel.x);
        double cutoff_frequency_y = min_cutoff_frequency + beta * abs(curr_vel.y);
        double alpha_x = get_alpha(dT, cutoff_frequency_x);
        double alpha_y = get_alpha(dT, cutoff_frequency_y);

        // filtering
        curr_pos.x = solve_low_pass_filter(received_pos.x, prev_pos.x, alpha_x);
        curr_pos.y = solve_low_pass_filter(received_pos.y, prev_pos.y, alpha_y);

        // ready for next
        T_prev = T;
        curr_acc = curr_vel - prev_vel;
        prev_vel = curr_vel;
        prev_pos = curr_pos;
    }
};

// struct
struct chaser_State
{
    sf::CircleShape chaser_shape;
    sf::Vector2f position;
};

// function
int get_port_number(const std::string& filename = "../.env")
{
    std::ifstream file(filename);
    if (!file.is_open()) return 0;

    std::string line;
    while (std::getline(file,line))
    {
        if (line.empty() || line[0] == '#') continue;

        size_t split_point = line.find('=');
        if (split_point != std::string::npos)
        {
            std::string key = line.substr(0, split_point - 1);
            std::string value = line.substr(split_point + 2);
            if (key == "port_number") return std::stoi(value);
        }
    }
    return -1;
}

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

int main()
{
    // Udp Socket 初期化
    sf::UdpSocket socket;
    std::size_t received;
    std::optional<sf::IpAddress> sender;
    unsigned short port;
    int port_number = get_port_number();
    socket.setBlocking(false);
    if (socket.bind(port_number) != sf::Socket::Status::Done) return -1;


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
    playerState player_state;
    sf::Vector2f received_pos;


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
            player.setPosition({received_pos.x * window_size.x, received_pos.y * window_size.y});
            // Player State Update
            player_state.update(received_pos);
        }


        // 敵の追跡アルゴリズム
        
        // 単純追跡
        // for (auto& chaser : chasers) chasing(chaser, player_state.get_curr_pos(), window_size);
        for (int i = 0; i < 4; i++)
        {
            if (i < 2) chasing(chasers[i], player_state.get_curr_pos(), window_size);
            else chasing(chasers[i], player_state.get_Euler_predict(), window_size);
        }


        


        // 敵への当たり判定
        for (const auto& chaser : chasers)
        {
            if (getDistance_square(player_state.get_curr_pos(), chaser.position, window_size) < ((player_size_rad + chaser_size_rad) * (player_size_rad + chaser_size_rad)))
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