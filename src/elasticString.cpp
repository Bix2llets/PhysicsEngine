#include "elasticString.h"

std::vector<sf::Vertex> ElasticString::getLineVertexArray() const {
    std::vector<sf::Vertex> result(2);

    result[0].color = sf::Color::White;
    result[1].color = sf::Color::White;
    if (!isBothObject) {
        result[0].position = anchor;
        result[1].position = object1->getPosition();
    } else {
        result[0].position = object1->getPosition();
        result[1].position = object2->getPosition();
    }
    return result;
}

void ElasticString::update() {
    using sf::Vector2f;
    Vector2f distanceVect;
    if (isBothObject)
        distanceVect = object1->getPosition() - object2->getPosition();
    else
        distanceVect = object1->getPosition() - anchor;
    if (distanceVect.length() <= desiredLength) return;

    Vector2f unit = distanceVect.normalized();

    object1->addForceImpact(-unit * stiffness *
                            (distanceVect.length() - desiredLength));
    if (isBothObject)
        object2->addForceImpact(unit * stiffness *
                                (distanceVect.length() - desiredLength));
}

void ElasticString::draw(sf::RenderTarget& target, sf::RenderStates state) const {
    std::vector<sf::Vertex> line = getLineVertexArray();
    target.draw(line.data(), line.size(), sf::PrimitiveType::Lines, state);
}

ElasticString::ElasticString(const sf::Vector2f& anchor, PhysicObject& obj,
                             float desiredLength, float stiffness)
    : anchor{anchor},
      object1{&obj},
      desiredLength{desiredLength},
      stiffness{stiffness},
      isBothObject{false} {};
ElasticString::ElasticString(PhysicObject& obj1, PhysicObject& obj2,
                             float desiredLength, float stiffness)
    : object1{&obj1},
      object2{&obj2},
      desiredLength{desiredLength},
      stiffness{stiffness},
      isBothObject{true} {};