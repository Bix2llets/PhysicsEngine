#pragma once
#include <SFML/Graphics.hpp>

#include "PhysicObject.h"

class Ball : public PhysicObject {
    // * Motion part
   public:
    // * Constructor and desstructor
    Ball(sf::Vector2f position, float mass = 1, float radius = 10,
         sf::Color color = sf::Color::White);
    Ball() : PhysicObject{sf::Vector2f{0.f, 0.f}, 0, false} {};

    // * GUIObject part
   private:
    sf::CircleShape base;
    bool isHeldLeft;
    bool handleLeftMousePressed(std::optional<sf::Event> &event,
                                sf::RenderWindow &window);
    bool handleLeftMouseReleased(std::optional<sf::Event> &event,
                                 sf::RenderWindow &window);
    void followCursor(sf::RenderWindow &window);
    void processLeftMouseHolding(sf::RenderWindow &window);

   public:
    void update();
    bool processEvent(std::optional<sf::Event> &event,
                      sf::RenderWindow &window);
    void processInput(sf::RenderWindow &window);
    void draw(sf::RenderTarget &target, sf::RenderStates state) const;

    inline float getRadius() const { return base.getRadius(); }
};