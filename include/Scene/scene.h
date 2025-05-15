#pragma once
#include <SFML/Graphics.hpp>

#include <optional>
class Scene {
   protected:
    sf::RenderWindow &window;

    Scene(sf::RenderWindow &window);
    Scene() = delete;
    virtual ~Scene() = default;

   public:
    virtual void render() = 0;
    virtual void update() = 0;
    virtual void processEvent(std::optional<sf::Event> &event) = 0;
    virtual void processInput() = 0;
};
