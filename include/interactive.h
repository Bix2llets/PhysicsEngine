#pragma once
#include <SFML/Graphics.hpp>

class Interactive {
    virtual bool processEvent(std::optional<sf::Event> &event,
                              sf::RenderWindow &window) = 0;
    virtual void processInput(sf::RenderWindow &window) = 0;
};