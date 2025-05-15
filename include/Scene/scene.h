#include <SFML/Graphics.hpp>

#include "GUIObject.h"

class Scene {
   protected:
    sf::RenderWindow &window;

    Scene(sf::RenderWindow &window);
    Scene() = delete;
    virtual ~Scene() = default;

   public:
    virtual void render() = 0;
    virtual void update() = 0;
    virtual void processEvent() = 0;
    virtual void processInput() = 0;
};
