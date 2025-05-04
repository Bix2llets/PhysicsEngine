#include "ball.h"
#include <math.h>
#include <iostream>
Ball::Ball(sf::Vector2f position, float radius, float mass, sf::Color color,
           sf::Color borderColor, float borderThickness)
    : isHolding{false}, mass{mass} {
    setRadius(radius);
    setOrigin(sf::Vector2f(radius, radius));
    
    setPosition(position);
    setFillColor(color);
    setOutlineColor(borderColor);
    setOutlineThickness(borderThickness);
}

Ball::Ball() : isHolding{false}, mass{1.f} {
    setRadius(10);
    setOrigin(sf::Vector2f(10, 10));
    setFillColor(sf::Color::White);
    setOutlineColor(sf::Color::Blue);
};

void Ball::update(sf::RenderWindow &window, float timeElapsed) {
    using sf::Vector2f;
    if (isHolding) {
        followCursor(window);
        velocity = sf::Vector2f(0.f, 0.f);
        // scale(sf::Vector2f(1.1f, 1.1f));
        return;
    }
    setScale(sf::Vector2f(1.f, 1.f));

    move(velocity * timeElapsed);
    clampPosition(Vector2f{0, 0}, Vector2f{window.getSize()});
}

void Ball::accelerate(sf::Vector2f acceleration, float timeElapsed) {
    velocity += acceleration * timeElapsed;
}

void Ball::handleEvent(sf::RenderWindow &window, const std::optional<sf::Event> &event) {
    using sf::Vector2f;
    namespace Mouse = sf::Mouse;
    using sf::Event;
    using std::cerr;
    if (isHolding) {
        const auto* mouseEvent = event->getIf<Event::MouseButtonReleased>();
        if (mouseEvent == nullptr) return;
        if (mouseEvent->button == Mouse::Button::Left)
        {
            isHolding = false;
            float speed = sqrt(accumulatedEnergy * 2 / mass);
            velocity = speed * previousDisplacement;
            accumulatedEnergy = 0;
            previousDisplacement = {0, 0};
        } 
        return;
    }
    // Vector2f tmp = Vector2f(Mouse::getPosition(window));
    Vector2f mousePosition = window.mapPixelToCoords(Mouse::getPosition(window));
    // cerr << mousePosition.x << " " << mousePosition.y << "\n";
    // cerr << tmp.x << " " << tmp.y << "\n";
    Vector2f currentPosition = getPosition();
    // cerr << currentPosition.x << " " << currentPosition.y << "\n";
    if ((mousePosition - currentPosition).length() > getRadius()) return;

    const auto* mouseEvent = event->getIf<Event::MouseButtonPressed>();
    if (mouseEvent == nullptr) return;
    if (mouseEvent->button == Mouse::Button::Left)
        isHolding = true;



}

void Ball::followCursor(sf::RenderWindow &window) {
    sf::Vector2f previousPosition = getPosition();
    setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    sf::Vector2f currentPosition = getPosition();

    sf::Vector2f displacement = currentPosition - previousPosition;
    float pathTravelled = displacement.length();
    accumulatedEnergy += pathTravelled;
    previousDisplacement = displacement.length() < 1e-6 ? sf::Vector2f{0.f, 0.f} : displacement.normalized() * 100.f;
}

void Ball::render(sf::RenderWindow &window) {
    window.draw(*this);
}

void Ball::processInput(sf::RenderWindow &window, const std::optional<sf::Event> &event) {
    handleEvent(window, event);
}

void Ball::clampPosition(sf::Rect<float> rect) {
    // Calibrate the rectangle
    rect.position += {getRadius(), getRadius()};
    rect.size -= {getRadius(), getRadius()};

    if (rect.contains(getPosition())) return;

    sf::Vector2f position = getPosition();
    if (position.y < rect.position.y)
    {
        position.y = rect.position.y;
        velocity.y *= -EDGE_BOUNCE_FACTOR;
    } 
    if (position.y > rect.position.y + rect.size.y) {
        position.y = rect.position.y + rect.size.y;
        velocity.y *= -EDGE_BOUNCE_FACTOR; 
    }
    if (position.x < rect.position.x)
    {
        position.x = rect.position.x;
        velocity.x *= -EDGE_BOUNCE_FACTOR;
    } 
    if (position.x > rect.position.x + rect.size.x) {
        position.x = rect.position.x + rect.size.x;
        velocity.x *= -EDGE_BOUNCE_FACTOR; 
    }
    setPosition(position);

} 

void Ball::clampPosition(sf::Vector2<float> position, sf::Vector2<float> size) {
    // Calibrate the rectangle
    // std::cerr << "Clamping\n";
    position += {getRadius(), getRadius()};
    size -= 2.0f * sf::Vector2f((float)getRadius(), (float)getRadius());
    sf::Rect<float> boundingRectangle(position, size);
    if (boundingRectangle.contains(getPosition())) return;
    sf::Vector2f currentPosition = getPosition();
    if (currentPosition.y < position.y)
    {
        currentPosition.y = position.y;
        velocity.y *= -EDGE_BOUNCE_FACTOR;
    } 
    if (currentPosition.y > position.y + size.y) {
        currentPosition.y = position.y + size.y;
        velocity.y *= -EDGE_BOUNCE_FACTOR; 
    }
    if (currentPosition.x < position.x)
    {
        currentPosition.x = position.x;
        velocity.x *= -EDGE_BOUNCE_FACTOR;
    } 
    if (currentPosition.x > position.x + size.x) {
        currentPosition.x = position.x + size.x;
        velocity.x *= -EDGE_BOUNCE_FACTOR; 
    }
    setPosition(currentPosition);

}