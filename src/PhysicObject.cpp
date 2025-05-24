#include "PhysicObject.h"
#include "info.h"
void PhysicObject::addVelocity(const sf::Vector2f &velocity) {
    previousPosition -= velocity * Info::SIMULATION_INTERVAL;
}

void PhysicObject::setVelocity(const sf::Vector2f &velocity) {
    previousPosition = position - velocity * Info::SIMULATION_INTERVAL;
}

void PhysicObject::addAcceleration(const sf::Vector2f &a) {
    acceleration += a;
}

void PhysicObject::addForceImpact(const sf::Vector2f &force) {
    acceleration += force / mass;
}

void PhysicObject::shift(const sf::Vector2f &displacement) {
    position += displacement;
    previousPosition += displacement;
}

void PhysicObject::move(const sf::Vector2f &displacement) {
    position += displacement;
}

sf::Vector2f PhysicObject::getVelocity() {
    return (position - previousPosition) / Info::SIMULATION_INTERVAL;
}