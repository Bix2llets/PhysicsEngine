#include "GUIObject.h"

class PhysicObject : public GUIObject {
    // * Virtual abstract methods
   public:
    virtual void draw(sf::RenderTarget& target,
                      sf::RenderStates state) const = 0;
    virtual bool processEvent(std::optional<sf::Event>& event,
                              sf::RenderWindow& window) = 0;
    virtual void processInput(sf::RenderWindow& window) = 0;
    virtual void update() = 0;

    // * Position, acceleration and related methods
   protected:
    bool isFixed;
    sf::Vector2f position;
    sf::Vector2f previousPosition;
    sf::Vector2f acceleration;
    float mass;

   public:
    // Getters
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getPreviousPosition() const { return previousPosition; }
    sf::Vector2f getAcceleration() const { return acceleration; }
    float getMass() const { return mass; }

    // Setters
    void setPosition(const sf::Vector2f& pos) { position = pos; }
    void setPreviousPosition(const sf::Vector2f& prevPos) {
        previousPosition = prevPos;
    }
    void setAcceleration(const sf::Vector2f& accel) { acceleration = accel; }
    void setMass(const float& m) { mass = m; }

    void addVelocity(const sf::Vector2f& velocity);
    void setVelocity(const sf::Vector2f& velocity);
    sf::Vector2f getVelocity();
    void addAcceleration(const sf::Vector2f& a);
    void addForceImpact(const sf::Vector2f& force);

    void shift(const sf::Vector2f& displacement);
    void move(const sf::Vector2f& displacement);

    // * Constructors and Destructor

    PhysicObject(sf::Vector2f position, float mass, bool isFixed)
        : position{position},
          mass{mass},
          isFixed{isFixed},
          previousPosition{position},
          acceleration{sf::Vector2f{0.f, 0.f}} {}

    ~PhysicObject() {}
};