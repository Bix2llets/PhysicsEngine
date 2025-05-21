#pragma once
#include <SFML/Graphics.hpp>

#include "GUIObject.h"

class Ball : public GUIObject {
    // * Motion part
   public:
    // * Constructor and desstructor
    Ball(sf::Vector2f position, float mass = 1, float radius = 10,
         sf::Color color = sf::Color::White);
    Ball() = default;

   private:
    sf::Vector2f position;
    sf::Vector2f previousPosition;
    sf::Vector2f acceleration;

   public:
    inline sf::Vector2f getPosition() const { return position; }
    inline sf::Vector2f getPreviousPosition() const { return previousPosition; }
    inline sf::Vector2f getAcceleration() const { return acceleration; }

    inline void setPosition(sf::Vector2f pos) {
        position = pos;
        base.setPosition(position);
    };
    inline void setPreviousPosition(sf::Vector2f pos) {
        previousPosition = pos;
    }
    void move(sf::Vector2f displacement);
    
    inline void setAcceleration(sf::Vector2f acc) { acceleration = acc; }

    void addForceImpact(sf::Vector2f force);
    void addAcceleration(sf::Vector2f acc);

    void shift(sf::Vector2f displacement);

    sf::Vector2f getVelocity();
    void addVelocity(sf::Vector2f velocity);
    void setVelocity(sf::Vector2f velocity);
    // * GUIObject part
   private:
    float mass;
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

    inline float getMass() const { return mass; }
    inline float getRadius() const { return base.getRadius(); }
};