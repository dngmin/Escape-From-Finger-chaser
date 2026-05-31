#pragma once
#include <SFML/Graphics.hpp>

struct GameConfig
{
    struct WindowConfig
    {
        const char* title = "Game";
        sf::Vector2u window_size = {800, 600};
        sf::Color background_color = sf::Color::Black;
    };

    struct EntityConfig
    {
        sf::Vector2f init_pos = {0.5f, 0.5f};
        float entity_size = 10.f;
        sf::Color color = sf::Color::White;
    };

    WindowConfig window_config;
    EntityConfig player_config;
    EntityConfig chaser1_config = {
        .init_pos = {0.1f, 0.1f},
        .color = sf::Color::Red
    };
    EntityConfig chaser2_config = {
        .init_pos = {0.9f, 0.1f},
        .color = sf::Color::Red
    };
    EntityConfig chaser3_config = {
        .init_pos = {0.1f, 0.9f},
        .color = sf::Color::Green
    };
    EntityConfig chaser4_config = {
        .init_pos = {0.9f, 0.9f},
        .color = sf::Color::Green
    };
};