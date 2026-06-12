#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// 本プログラム開始に必要な全ての初期値を管理している
// 0~1の比率で正規化した座標系を使用するため初期位置も0~1の値にする
struct SimulationConfig
{
    struct WindowConfig
    {
        const char* title = "Simulation";
        sf::Vector2u window_size = {800, 600};
        sf::Color background_color = sf::Color::Black;
    };

    struct EntityConfig
    {
        sf::Vector2f init_pos = {0.5f, 0.5f};
        float entity_size = 10.f;
        sf::Color color = sf::Color::White;
    };

    struct ChasingObjectConfig
    {
        size_t ChasingObjectCount = 0;
        std::vector<EntityConfig> chasing_objects;
        int prediction_steps = 5;
        float speed = 5e-5;
    };

    WindowConfig window_config;

    EntityConfig player_config;

    EntityConfig ChasingObject1_config = {
        .init_pos = {0.1f, 0.1f},
        .color = sf::Color::Red
    };
    EntityConfig ChasingObject2_config = {
        .init_pos = {0.9f, 0.1f},
        .color = sf::Color::Green
    };
    EntityConfig ChasingObject3_config = {
        .init_pos = {0.1f, 0.9f},
        .color = sf::Color::Yellow
    };
    EntityConfig ChasingObject4_config = {
        .init_pos = {0.9f, 0.9f},
        .color = sf::Color::Cyan
    };

    ChasingObjectConfig chasing_objects_config = {
        .ChasingObjectCount = 4,
        .chasing_objects = {
            ChasingObject1_config,
            ChasingObject2_config,
            ChasingObject3_config,
            ChasingObject4_config
        }
    };
};

enum class Stage
{
    SIMULTATING = 0,
    CLOSISING = 1
};