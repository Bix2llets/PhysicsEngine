#include "ball.h"

#include <math.h>

#include <iostream>

#include "info.h"
Ball::Ball(sf::Vector2f position, float radius, float mass, sf::Color color,
           sf::Color borderColor, float borderThickness)
    : isHeldLeft{false}, mass{mass} {
    base.setRadius(radius);
    base.setOrigin(sf::Vector2f(radius, radius));

    base.setPosition(position);
    previousPosition = position;
    base.setFillColor(color);
    base.setOutlineColor(borderColor);
    base.setOutlineThickness(borderThickness);
}

Ball::Ball() : isHeldLeft{false}, mass{1.f} {
    base.setRadius(10);
    base.setOrigin(sf::Vector2f(10, 10));
    base.setFillColor(sf::Color::White);
    base.setOutlineColor(sf::Color::Blue);
};

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
    sf::Vector2f nextPosition = 2.f * base.getPosition() - previousPosition +
                                0.5f * acceleration *
                                    Info::POLLING_INTERVAL *
                                    Info::POLLING_INTERVAL;
    previousPosition = base.getPosition();
    base.setPosition(nextPosition);

    acceleration = {0.f, 0.f};
}

void Ball::followCursor(sf::RenderWindow &window) {
    sf::Vector2f previousPosition = base.getPosition();
    base.setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
    sf::Vector2f currentPosition = base.getPosition();

    sf::Vector2f displacement = currentPosition - previousPosition;
    float pathTravelled = displacement.length();
    accumulatedEnergy += pathTravelled;
    previousDisplacement = displacement.length() < 1e-6
                               ? sf::Vector2f{0.f, 0.f}
                               : displacement.normalized() * 100.f;
}

void Ball::move(sf::Vector2f displacement) { base.move(displacement); }

bool Ball::handleLeftMousePressed(std::optional<sf::Event> &event,
                                  sf::RenderWindow &window) {
    if (isHeldLeft) return false;
    const auto *mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
    if (mouseEvent == nullptr) return false;
    if (mouseEvent->button != sf::Mouse::Button::Left) return false;
    sf::Vector2f mousePosition = window.mapPixelToCoords(mouseEvent->position);

    if ((mousePosition - base.getPosition()).length() > base.getRadius())
        return false;

    isHeldLeft = true;
    previousPosition = base.getPosition();

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
    float speedGained = sqrt(accumulatedEnergy * 2 / mass);
    accumulatedEnergy = 0;
    if (previousDisplacement.length() < 1e-6) return true;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) return true;
    sf::Vector2f initialVelocity = previousDisplacement * speedGained;
    
    base.setPosition(base.getPosition());
    
    previousPosition = base.getPosition() - initialVelocity * Info::POLLING_INTERVAL;
    // std::cerr << "Launched the ball\n";
    return true;
}

void Ball::processLeftMouseHolding(sf::RenderWindow &window) {
    if (isHeldLeft) {  // * Follow mouse cursor while being held
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);
        previousDisplacement = worldPosition - base.getPosition();
        if (previousDisplacement.length() < 1e-6)
            previousDisplacement = {0.f, 0.f};
        else
            previousDisplacement = previousDisplacement.normalized();
        accumulatedEnergy += previousDisplacement.length();

        previousDisplacement *= 100.f;
        // std::cerr << accumulatedEnergy << "\n";
        base.setPosition(worldPosition);
        previousPosition = worldPosition;  // * So the velocity is zero;
        acceleration = {0.f, 0.f};
    }
}

void Ball::setPosition(sf::Vector2f newPosition) {
    base.setPosition(newPosition);
}

void Ball::addForceImpact(sf::Vector2f force) {
    sf::Vector2f forceAcceleration = force / mass;
    acceleration += forceAcceleration;
}

void Ball::setPreviousPosition(sf::Vector2f position) {
    previousPosition = position;
}

sf::Vector2f Ball::getVelocity() {
    return (base.getPosition() - previousPosition) / Info::POLLING_INTERVAL;
}

void Ball::setVelocity(sf::Vector2f velocity) {
    previousPosition = base.getPosition() - velocity * Info::POLLING_INTERVAL;
}