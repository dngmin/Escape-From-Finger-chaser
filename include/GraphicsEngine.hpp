#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.hpp"
#include "SimulationConfig.hpp"
#include <vector>

class GraphicsEngine
{
public:
    GraphicsEngine(const SimulationConfig::WindowConfig& window_config);

    // ユーザーからのwindow終了、リサイズ情報を更新
    void UpdateWindowEvent();

    void Render(const Entity& player, const std::vector<Entity>& chasing_objects);

    sf::RenderWindow window;
    const char* window_title;
    sf::Vector2u window_size;
    sf::Color background_color;
private:
};