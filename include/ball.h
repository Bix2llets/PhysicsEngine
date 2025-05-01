#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

class Ball : public sf::CircleShape {
   private:
    static constexpr float GRAVITY = 9800.f;
    sf::Vector2f velocity;
    bool isHolding;
    float mass;
    static constexpr float EDGE_BOUNCE_FACTOR = 0.5f;
   public:
    Ball(sf::Vector2f position, float radius = 10.f, float mass = 1.f,
         sf::Color color = sf::Color::White,
         sf::Color borderColor = sf::Color::White, float borderThickness = 2.);

    Ball();

    void render(sf::RenderWindow &window);

    void processInput(sf::RenderWindow &window,
                      const std::optional<sf::Event> &event);

    void update(sf::RenderWindow &window, float timeElapsed);

    void accelerate(sf::Vector2f acceleration, float timeElapsed);

    inline void setVelocity(sf::Vector2f newVelocity) {
        velocity = newVelocity;
    }

    inline sf::Vector2f getVelocity() const { return velocity; }

    inline void setMass(float newMass) { mass = newMass; }
    inline float getMass() { return mass; }
    void processMouseEvent(sf::RenderWindow &window,
                           const std::optional<sf::Event> &event);

    void followCursor(sf::RenderWindow &window);

    void clampPosition(sf::Rect<float> rect);
    void clampPosition(sf::Vector2<float> position, sf::Vector2<float> size);
};