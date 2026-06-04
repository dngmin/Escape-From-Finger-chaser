#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// ゲーム開始に必要な全ての初期値を管理している
// 0~1の比率で正規化した座標系を使用するため初期位置も0~1の値にする
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

    struct ChasersConfig
    {
        size_t chaserCount = 0;
        std::vector<EntityConfig> chasers;
        int prediction_steps = 5;
        float speed = 5e-5;
    };

    WindowConfig window_config;

    EntityConfig player_config;

    EntityConfig chaser1_config = {
        .init_pos = {0.1f, 0.1f},
        .color = sf::Color::Red
    };
    EntityConfig chaser2_config = {
        .init_pos = {0.9f, 0.1f},
        .color = sf::Color::Green
    };
    EntityConfig chaser3_config = {
        .init_pos = {0.1f, 0.9f},
        .color = sf::Color::Green
    };
    EntityConfig chaser4_config = {
        .init_pos = {0.9f, 0.9f},
        .color = sf::Color::Cyan
    };

    ChasersConfig chasers_config = {
        .chaserCount = 4,
        .chasers = {
            chaser1_config,
            chaser2_config,
            chaser3_config,
            chaser4_config
        }
    };
};