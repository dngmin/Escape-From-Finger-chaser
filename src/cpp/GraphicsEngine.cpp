#include "GraphicsEngine.hpp"

GraphicsEngine::GraphicsEngine(const SimulationConfig::WindowConfig& window_config)
    : window(sf::VideoMode(window_config.window_size), window_config.title)
    , window_title(window_config.title)
    , window_size(window_config.window_size)
    , background_color(window_config.background_color)
    , font("../assets/NotoSansJP-Bold.ttf")
    , closing_text(font,"press 'Q' to exit / 'R' to restart")
{
    // closing_textサイズ初期化
    ResizeClosingText();
};

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
            ResizeClosingText();
        }
    }
}

void GraphicsEngine::Render(const Entity& player, const std::vector<Entity>& chasing_objects, Stage stage)
{
    window.clear(background_color);
    window.draw(player.entity);
    for (auto& chasing_object : chasing_objects) window.draw(chasing_object.entity);

    if (stage == Stage::AWAIT_USER_INPUT) window.draw(closing_text);

    window.display();
}

void GraphicsEngine::ResizeClosingText(unsigned int divisor)
{
    unsigned int font_size = window_size.x / divisor;
    closing_text.setCharacterSize(font_size);
}