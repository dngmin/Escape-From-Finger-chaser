#include "GraphicsEngine.hpp"

GraphicsEngine::GraphicsEngine(const char* title, const sf::Vector2u& init_size, const sf::Color color)
    : window(sf::VideoMode(init_size), title)
    , window_title(title)
    , window_size(init_size)
    , background_color(color)
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