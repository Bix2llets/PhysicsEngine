#include "ball.h"

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
    if (isHolding) {
        followCursor(window);
        velocity = sf::Vector2f(0.f, 0.f);
        // scale(sf::Vector2f(1.1f, 1.1f));
        return;
    }
    setScale(sf::Vector2f(1.f, 1.f));

    move(velocity * timeElapsed);
}

void Ball::accelerate(sf::Vector2f acceleration, float timeElapsed) {
    velocity += acceleration * timeElapsed;
}

void Ball::processMouseEvent(sf::RenderWindow &window, const std::optional<sf::Event> &event) {
    using sf::Vector2f;
    namespace Mouse = sf::Mouse;
    using sf::Event;
    using std::cerr;
    if (isHolding) {
        const auto* mouseEvent = event->getIf<Event::MouseButtonReleased>();
        if (mouseEvent == nullptr) return;
        if (mouseEvent->button == Mouse::Button::Left) 
            isHolding = false;
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
    setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
}

void Ball::render(sf::RenderWindow &window) {
    window.draw(*this);
}

void Ball::processInput(sf::RenderWindow &window, const std::optional<sf::Event> &event) {
    processMouseEvent(window, event);
}
