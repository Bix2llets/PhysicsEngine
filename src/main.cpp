#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cstdio>
#include <iostream>
#include <vector>

#include "GUI/sliderBar.h"
#include "ball.h"
const float UPDATE_INTERVAL = 1.0f / 60;
const float RENDER_INTERVAL = 1.0f / 30;
float COLLISION_ENERGY_CONSERVATION_RATIO = 1.f;
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
SliderBar slider({30, 30}, {100, 20}, {1.0f}, {0.f, 1.f});

void resolveCollision() {
    using sf::Vector2f;
    using sf::Vector2i;

    using V2f = sf::Vector2f;
    for (int i = 0; i < ballList.size(); i++)
        for (int j = i + 1; j < ballList.size(); j++) {
            if (i == j) continue;
            Ball &ball1 = ballList[i];
            Ball &ball2 = ballList[j];

            Vector2f positionDifference =
                ball2.getPosition() - ball1.getPosition();

            float ballDistance = positionDifference.length();

            if (ballDistance >
                ball1.getRadius() + ball2.getRadius())  // Separated
                continue;

            float overlapped =
                ball1.getRadius() + ball2.getRadius() - ballDistance;
            // Pointing from ball1 to ball2
            Vector2f normalVector;

            if (positionDifference.length() <
                1e-6)  // The length approaching zero in floating point
                       // calculation
                normalVector = {1, 0};
            else
                normalVector = positionDifference.normalized();

            float mass1 = ball1.getMass();
            float mass2 = ball2.getMass();
            // Positioning balls to remove sinking
            ball1.move(-1.0f * normalVector * overlapped *
                       (mass1 / (mass1 + mass2)));
            ball2.move(1.0f * normalVector * overlapped *
                       (mass2 / (mass1 + mass2)));

            V2f relativeVelocity = ball1.getVelocity() - ball2.getVelocity();

            /*
             The frame of reference is on ball2. During which it sees that ball1
             is moving at the relativeVelocity
             */

            float collisionSpeed = relativeVelocity.dot(normalVector);
            if (collisionSpeed <= 0) continue;
            // The trajectory of the ball is colliding. Apply change in velocity
            // to simulate force impact

            // assert(-1.f <= COLLISION_ENERGY_CONSERVATION_RATIO &&
            // BOUNCE_FACTOR <= 1.f);

            float speedChange =
                collisionSpeed * (COLLISION_ENERGY_CONSERVATION_RATIO);

            V2f deltaVelocity1 = -speedChange * normalVector;

            ball1.setVelocity(ball1.getVelocity() + deltaVelocity1);

            V2f impulseBall2 = deltaVelocity1 * ball1.getMass() * -1.f;

            V2f deltaVelocity2 = impulseBall2 / ball2.getMass();

            ball2.setVelocity(ball2.getVelocity() + deltaVelocity2);
        }
}
void Update(sf::Time elapsed) {
    remainingTime += elapsed.asSeconds();
    while (remainingTime > UPDATE_INTERVAL) {
        // std::cout << remainingTime << "\n";
        remainingTime -= UPDATE_INTERVAL;
        for (auto &ball : ballList)
            ball.accelerate(sf::Vector2f(0, 980.f), UPDATE_INTERVAL);
        for (auto &ball : ballList) ball.update(window, UPDATE_INTERVAL);
        for (int i = 0; i < 3; i++) resolveCollision();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            for (auto &ball : ballList)
                ball.accelerate(sf::Vector2f(1000, 0), UPDATE_INTERVAL);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            for (auto &ball : ballList)
                ball.accelerate(sf::Vector2f(0, -1000), UPDATE_INTERVAL);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            for (auto &ball : ballList)
                ball.accelerate(sf::Vector2f(0, 1000), UPDATE_INTERVAL);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            for (auto &ball : ballList)
                ball.accelerate(sf::Vector2f(-1000, 0), UPDATE_INTERVAL);

        slider.update(window);
        COLLISION_ENERGY_CONSERVATION_RATIO = slider.getValue();
    }
}

void processInput(sf::RenderWindow &window,
                  const std::optional<sf::Event> &event) {
    for (auto &ball : ballList) ball.processInput(window, event);
    slider.handleEvent(window, event);
}
int main() {
    srand(time(NULL));
    sf::Vector2<int> vect;
    window.setFramerateLimit(60);
    window.create(sf::VideoMode({1000, 900}), "Billreal's Physic Engine",
                  sf::Style::Default);

    sf::Clock clock;
    sf::View view;

    sf::CircleShape circle(50, 30);
    circle.setOrigin({50, 50});
    circle.setPosition({100, 100});
    for (int i = 0; i < 100; i++) {
        sf::Vector2f position = {(float)(rand() % window.getSize().x),
                                 (float)(rand() % window.getSize().y)};
        int colorIndex = rand() % colorList.size();
        ballList.push_back(Ball(position, 30, 1.f, colorList[colorIndex],
                                sf::Color::Black, 0.5f));
    }
    while (window.isOpen()) {
        // sf::Vector2f mousePosition =
        // sf::Vector2f(sf::Mouse::getPosition(window)); std::cerr <<
        // mousePosition.x << " " << mousePosition.y << "\n";
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (event->is<sf::Event::KeyPressed>()) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
                    window.close();
            } else {
                processInput(window, event);
                for (auto &ball : ballList) ball.processInput(window, event);
            }
        }
        using sf::Vector2f;
        namespace Mouse = sf::Mouse;
        // ball.move({0.1f, 0.1f});
        // Vector2f mouseWindowPos = Vector2f(Mouse::getPosition(window));

        // Vector2f actualPosition =
        //     window.mapPixelToCoords(Mouse::getPosition(window));
        // std::cerr << mouseWindowPos.x << " " << mouseWindowPos.y << "\n";
        // std::cerr << actualPosition.x << " " << actualPosition.y << "\n";
        // std::cerr << "\n";
        // ball.rotate(sf::radians(0.1));
        // ball.rotate(sf::radians(0.001));
        sf::Time timePassed = clock.getElapsedTime();

        float FPSCount = 1.f / timePassed.asSeconds();
        window.setTitle("Physic Engine. FPS: " + std::to_string((int)FPSCount));
        Update(clock.restart());

        view.setSize(sf::Vector2f(window.getSize()));
        view.setCenter(sf::Vector2f(window.getSize()) / 2.f);

        window.setView(view);
        window.clear(sf::Color::Black);

        for (auto &ball : ballList) ball.render(window);
        slider.render(window);
        // for (auto &ball : ballList) window.draw(ball);
        window.display();
    }
    vect.x = 1;
    vect.y = 2;
    std::cout << vect.x << " " << vect.y << "\n";
}