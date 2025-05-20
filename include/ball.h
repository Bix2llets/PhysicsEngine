#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include "GUIObject.h"
class Ball : public GUIObject {
   private:
    sf::CircleShape base;
    static constexpr float GRAVITY = 9800.f;
    sf::Vector2f acceleration;
    bool isHeldLeft;
    float mass;
    static constexpr float EDGE_BOUNCE_FACTOR = 0.5f;

    sf::Vector2f previousPosition;
    float accumulatedEnergy = 0.f;
    sf::Vector2f previousDisplacement;

    bool handleLeftMousePressed( std::optional<sf::Event> &event, sf::RenderWindow &window);
    bool handleLeftMouseReleased(std::optional<sf::Event> &event, sf::RenderWindow &window);

    void processLeftMouseHolding(sf::RenderWindow &window);

   public:
    Ball(sf::Vector2f position, float radius = 10.f, float mass = 1.f,
         sf::Color color = sf::Color::White,
         sf::Color borderColor = sf::Color::White, float borderThickness = 2.);

    Ball();

    void processInput(sf::RenderWindow &window);
    bool processEvent(std::optional<sf::Event> &event, sf::RenderWindow &window);
    void draw(sf::RenderTarget &target,
                      sf::RenderStates state) const;
    void update();


    inline void addAcceleration(sf::Vector2f accelerateValue) {
        acceleration += accelerateValue;
    }

    void addForceImpact(sf::Vector2f force);


    inline void setMass(float newMass) { mass = newMass; }
    inline float getMass() { return mass; }

    void followCursor(sf::RenderWindow &window);

    inline float getRadius() const { return base.getRadius(); }
    inline sf::Vector2f getPosition() const { return base.getPosition(); };

    void move(sf::Vector2f displacement);
    void setPreviousPosition(sf::Vector2f position);
    void setPosition(sf::Vector2f newPosition);

    sf::Vector2f getVelocity();
    void setVelocity(sf::Vector2f velocity);
};