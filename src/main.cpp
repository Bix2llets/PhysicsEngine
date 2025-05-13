#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cstdio>
#include <iostream>
#include <vector>

#include "GUI/sliderBar.h"
#include "ball.h"
#include "info.h"
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

            continue;
        }

    // * Collision with the border of the screen

    for (Ball &ball : ballList) {
        sf::Vector2f position = ball.getPosition();
        sf::Vector2f velocity = ball.getVelocity();
        float radius = ball.getRadius();
        if (position.x > Info::SCREEN_WIDTH - radius) {
            position.x = Info::SCREEN_WIDTH - radius;
            velocity.x *= -0.8;
        }
        if (position.y > Info::SCREEN_HEIGHT - radius) {
            position.y = Info::SCREEN_HEIGHT - radius;
            velocity.y *= -0.8;
        }
        if (position.x < 0 + radius) {
            position.x = 0 + radius;
            velocity.x *= -0.8;
        }
        if (position.y < 0 + radius) {
            position.y = 0 + radius;
            velocity.y *= -0.8;
        }
        ball.setPosition(position);
        ball.setVelocity(velocity);
    }
}

void processEvent(std::optional<sf::Event> &event, sf::RenderWindow &window) {
    for (auto &ball : ballList) ball.processEvent(event, window);
}

void processInput(sf::RenderWindow &window) {
    for (auto &ball : ballList) ball.processInput(window);
}

void update() {
    for (auto &ball : ballList) ball.update();
    for (int i = 0; i < 3; i++) resolveCollision();
}

void render(sf::RenderWindow &window) {
    for (auto &ball : ballList) window.draw(ball);
}
int main() {
    srand(time(NULL));
    sf::Vector2<int> vect;
    window.setFramerateLimit(60);
    window.create(sf::VideoMode({Info::SCREEN_WIDTH, Info::SCREEN_HEIGHT}),
                  "Billreal's Physic Engine", sf::Style::Default);

    sf::Clock clock;
    sf::View view;

    for (int i = 0; i < 50; i++) {
        sf::Vector2f position = {(float)(rand() % window.getSize().x),
                                 (float)(rand() % window.getSize().y)};
        int colorIndex = rand() % colorList.size();
        ballList.push_back(
            Ball(position, 30, 1, sf::Color::Red, sf::Color::Black, 0.5f));
        ballList.push_back(
            Ball(position, 30, 1, sf::Color::Cyan, sf::Color::Black, 0.5f));
    }
    float elapsedTime = 0;
    int frameCount = 0;
    while (window.isOpen()) {
        elapsedTime += clock.getElapsedTime().asSeconds();
        clock.restart();
        for (; elapsedTime >= Info::SIMULATION_INTERVAL;
             elapsedTime -= Info::SIMULATION_INTERVAL) {
            // frameCount++;
            // std::cerr << "Frame: " << frameCount << "\n";
            while (std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) window.close();

                processEvent(event, window);
            }

            processInput(window);

            update();
        }
        window.clear(sf::Color::Black);
        render(window);
        window.display();
    }
}