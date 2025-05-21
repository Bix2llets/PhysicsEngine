#include "ball.h"

#include <math.h>

#include <iostream>

#include "info.h"

Ball::Ball(sf::Vector2f position, float mass, float radius, sf::Color color)
    : position{position}, previousPosition{position}, mass{mass}, isHeldLeft{false} {
    base.setOrigin({radius, radius});
    base.setPosition(position);
    base.setRadius(radius);
    base.setFillColor(color);
}
void Ball::draw(sf::RenderTarget &target, sf::RenderStates state) const {
    target.draw(base, state);
}

void Ball::processInput(sf::RenderWindow &window) {
    processLeftMouseHolding(window);
}

bool Ball::processEvent(std::optional<sf::Event> &event,
                        sf::RenderWindow &window) {
    bool processResult = false;
    processResult |= handleLeftMousePressed(event, window);
    processResult |= handleLeftMouseReleased(event, window);
    return processResult;
}

void Ball::update() {
    if (isHeldLeft) return;
    sf::Vector2f nextPosition =
        2.f * position - previousPosition +
        acceleration * Info::SIMULATION_INTERVAL * Info::SIMULATION_INTERVAL;
    previousPosition = position;
    position = nextPosition;
    base.setPosition(position);

    acceleration = {0.f, 0.f};
}

void Ball::followCursor(sf::RenderWindow &window) {
    previousPosition = position;
    position = (window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    base.setPosition(position);
}

bool Ball::handleLeftMousePressed(std::optional<sf::Event> &event,
                                  sf::RenderWindow &window) {
    if (isHeldLeft) return false;
    const auto *mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
    if (mouseEvent == nullptr) return false;
    if (mouseEvent->button != sf::Mouse::Button::Left) return false;
    sf::Vector2f mousePosition = window.mapPixelToCoords(mouseEvent->position);

    if ((mousePosition - position).length() > base.getRadius())
        return false;

    isHeldLeft = true;
    previousPosition = position;

    return true;
}

bool Ball::handleLeftMouseReleased(std::optional<sf::Event> &event,
                                   sf::RenderWindow &window) {
    if (!isHeldLeft) return false;
    const auto *mouseEvent = event->getIf<sf::Event::MouseButtonReleased>();
    if (mouseEvent == nullptr) return false;
    if (mouseEvent->button != sf::Mouse::Button::Left) return false;

    isHeldLeft = false;
    // std::cerr << previousDisplacement.length() << "\n";
    return true;
}

void Ball::processLeftMouseHolding(sf::RenderWindow &window) {
    if (isHeldLeft) {  // * Follow mouse cursor while being held
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);

        previousPosition = position;
        position = worldPosition;
        base.setPosition(position);
        acceleration = {0.f, 0.f};
    }
}

void Ball::addForceImpact(sf::Vector2f force) { acceleration += force / mass; }
void Ball::addAcceleration(sf::Vector2f acc) { acceleration += acc; }
void Ball::shift(sf::Vector2f displacement) {
    position += displacement;
    previousPosition += displacement;
}

sf::Vector2f Ball::getVelocity() {
    return (position - previousPosition) / Info::SIMULATION_INTERVAL;
}
void Ball::setVelocity(sf::Vector2f velocity) {
    previousPosition = position - velocity * Info::SIMULATION_INTERVAL;
}
void Ball::addVelocity(sf::Vector2f velocity) {
    previousPosition -= velocity * Info::SIMULATION_INTERVAL;
}

void Ball::move(sf::Vector2f displacement) {
    position += displacement;
}