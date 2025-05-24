#include <SFML/Graphics.hpp>
#include <vector>
#include "GUIObject.h"
#include "PhysicObject.h"
class ElasticString : public GUIObject {
    
    private:
    float desiredLength;
    float stiffness;
    PhysicObject* object1;
    PhysicObject* object2;
    bool isBothObject;
    sf::Vector2f anchor;

    std::vector<sf::Vertex> getLineVertexArray() const;
    public:
    ElasticString(const sf::Vector2f& anchor,  PhysicObject &obj, float desiredLength, float stiffness);
    ElasticString( PhysicObject& obj1,  PhysicObject& obj2, float desiredLength, float stiffness);

    void update();
    void draw(sf::RenderTarget &target, sf::RenderStates state) const;
};