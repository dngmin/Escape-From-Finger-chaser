#pragma once
#include <SFML/Graphics.hpp>
#include <Entity.hpp>
#include <vector>

class GraphicsEngine
{
public:
    GraphicsEngine(const char* title, const sf::Vector2u& init_size, const sf::Color color);

    // ユーザーからのwindow終了、リサイズ情報を更新
    void UpdateWindowEvent();

    void Render(const Entity& player, const std::vector<Entity>& chasers);

    sf::RenderWindow window;
    const char* window_title;
    sf::Vector2u window_size;
    sf::Color background_color;
private:
};