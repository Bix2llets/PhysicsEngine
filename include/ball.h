#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include "GUIObject.h"
class Ball : public GUIObject {
   private:
    sf::CircleShape base;
    static constexpr float GRAVITY = 9800.f;
    sf::Vector2f velocity;
    bool isHeldLeft;
    float mass;
    static constexpr float EDGE_BOUNCE_FACTOR = 0.5f;

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

    void accelerate(sf::Vector2f acceleration);

    inline void setVelocity(sf::Vector2f newVelocity) {
        velocity = newVelocity;
    }

    inline sf::Vector2f getVelocity() const { return velocity; }

    inline void setMass(float newMass) { mass = newMass; }
    inline float getMass() { return mass; }

    void followCursor(sf::RenderWindow &window);

    inline float getRadius() const { return base.getRadius(); }
    inline sf::Vector2f getPosition() const { return base.getPosition(); };

    void move(sf::Vector2f displacement);
    void setPosition(sf::Vector2f newPosition);
};