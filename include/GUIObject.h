#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
class GUIObject : public sf::Drawable {
    virtual void draw(sf::RenderTarget &target,
                      sf::RenderStates state) const = 0;

    virtual void update() = 0;
};