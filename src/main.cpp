#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cstdio>
#include <iostream>
#include <vector>

#include "GUI/sliderBar.h"
#include "Scene/BallScene.h"
#include "Scene/scene.h"
#include "ball.h"
#include "info.h"
const float UPDATE_INTERVAL = 1.0f / 60;
const float RENDER_INTERVAL = 1.0f / 30;
float ballCollisionConservationRatio = 1.f;
float wallCollisionConservationRatio = 1.f;
// ! Should be between -1.f and 0.f
// ! 0 means energy is conserved
// ! < 0 means energy is lost
// ! > 0 or < -1 means energy is gained via handwaving it into reality
sf::Color Red = sf::Color::Red;
sf::Color Black = sf::Color::Black;
sf::Color White = sf::Color::White;
sf::Color Green = sf::Color::Green;
sf::Color Magenta = sf::Color::Magenta;
sf::Color Cyan = sf::Color::Cyan;
sf::Color Blue = sf::Color::Blue;
sf::Color Yellow = sf::Color::Yellow;
std::vector<sf::Color> colorList = {Yellow,  Red,  White, Green,
                                    Magenta, Cyan, Blue};
// TODO: Add friction
sf::RenderWindow window;
float remainingTime;
std::vector<Ball> ballList;
SliderBar ballBounceSlider({30, 30}, {100, 20}, {1.0f}, {0.f, 1.f});
SliderBar wallBounceSlider({30, 100}, {100, 20}, {1.0f}, {0.f, 1.f});

int main() {
    srand(time(NULL));
    sf::Vector2<int> vect;
    window.setFramerateLimit(60);
    window.create(sf::VideoMode({Info::SCREEN_WIDTH, Info::SCREEN_HEIGHT}),
                  "Billreal's Physic Engine", sf::Style::Default);

    sf::Clock clock;
    sf::View view;

    BallScene ballScene(window, {0.f, 0.f}, sf::Vector2f{window.getSize()});
    Scene *currentScene = &ballScene;
    float elapsedTime = 0;
    int frameCount = 0;
    while (window.isOpen()) {
        float timeSinceLastFrame = clock.getElapsedTime().asSeconds();
        elapsedTime += timeSinceLastFrame;
        clock.restart();
        int countFrame = 0;
        for (; elapsedTime >= Info::POLLING_INTERVAL && countFrame < 2;
             elapsedTime -= Info::POLLING_INTERVAL, countFrame++) {
            // frameCount++;
            // std::cerr << "Frame: " << frameCount << "\n";
            while (std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>())
                    window.close();
                else {
                    currentScene->processEvent(event);
                }
            }

            for (int i = Info::POLLING_INTERVAL / Info::SIMULATION_INTERVAL;
                 i > 0; i--) {
                currentScene->processInput();
                currentScene->update();
            }

            window.setTitle("Physics Engine. Total energy: " +
                            std::to_string(ballScene.getEnergy() * 1e-6) + "FPS: " +
                            std::to_string(1.0f / timeSinceLastFrame));
        }
        window.clear(sf::Color::Black);
        currentScene->render();
        window.display();
    }
}