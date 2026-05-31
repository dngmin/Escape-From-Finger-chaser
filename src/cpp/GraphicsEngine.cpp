#include "GraphicsEngine.hpp"

GraphicsEngine::GraphicsEngine(const GameConfig::WindowConfig& window_config)
    : window(sf::VideoMode(window_config.window_size), window_config.title)
    , window_title(window_config.title)
    , window_size(window_config.window_size)
    , background_color(window_config.background_color)
{};

void GraphicsEngine::UpdateWindowEvent()
{
    while (std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>()) window.close();

        else if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            // ユーザーによるwindowリサイズが確認されると、変更されたサイズに改めてフォーカスを合わせる
            window_size = window.getSize();
            sf::View view(sf::FloatRect({0.f, 0.f}, sf::Vector2f(window_size)));
            window.setView(view);
        }
    }
}

void GraphicsEngine::Render(const Entity& player, const std::vector<Entity>& chasers)
{
    window.clear(background_color);
    window.draw(player.entity);
    for (auto& chaser : chasers) window.draw(chaser.entity);
    window.display();
}