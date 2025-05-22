#include "Scene/BallScene.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

#include "Scene/scene.h"
#include "info.h"
const std::vector<float> BallScene::GRAVITY_STRENGTH_DIVISION = {1.f};
const std::vector<float> BallScene::GRAVITY_STRENGTH_VALUES = {-980.f, 980.f};
const sf::Vector2f BallScene::GRAVITY_BAR_POSITION = {10.f, 10.f};

const std::vector<float> BallScene::BALL_BOUNCE_DIVISION = {1.f};
const std::vector<float> BallScene::BALL_BOUNCE_VALUES = {0.f, 2.f};
const sf::Vector2f BallScene::BALL_BAR_POSITION = {10.f, 70.f};

const std::vector<float> BallScene::WALL_BOUNCE_DIVISION = {1.f};
const std::vector<float> BallScene::WALL_BOUNCE_VALUES = {0.f, 2.f};
const sf::Vector2f BallScene::WALL_BAR_POSITION = {10.f, 130.f};

const sf::Vector2f BallScene::BAR_DIMENSION = {100.f, 10.f};
const float BallScene::ACCELERATE_FORCE = 980.f;
BallScene::BallScene(sf::RenderWindow &window, sf::Vector2f position,
                     sf::Vector2f size)
    : Scene(window),
      position{position},
      size{size},
      gravityStrengthSlider{GRAVITY_BAR_POSITION, BAR_DIMENSION,
                            GRAVITY_STRENGTH_DIVISION, GRAVITY_STRENGTH_VALUES},
      ballBounceSlider{BALL_BAR_POSITION, BAR_DIMENSION, BALL_BOUNCE_DIVISION,
                       BALL_BOUNCE_VALUES},
      wallBounceSlider{WALL_BAR_POSITION, BAR_DIMENSION, WALL_BOUNCE_DIVISION,
                       WALL_BOUNCE_VALUES} {}

void BallScene::render() {
    for (auto &ball : ballList) window.draw(ball);
    window.draw(gravityStrengthSlider);
    window.draw(ballBounceSlider);
    window.draw(wallBounceSlider);
}

void BallScene::update() {
    for (auto &ball : ballList)
        ball.addForceImpact(
            {0.f, gravityStrengthSlider.getValue() * ball.getMass()});
    for (auto &ball : ballList) ball.update();
    gravityStrengthSlider.update();
    ballBounceSlider.update();
    wallBounceSlider.update();

    resolveCollision();
}

void BallScene::processEvent(std::optional<sf::Event> &event) {
    if (ballBounceSlider.processEvent(event, window)) return;
    if (wallBounceSlider.processEvent(event, window)) return;
    if (gravityStrengthSlider.processEvent(event, window)) return;
    bool isBallEventProcessed = false;
    for (auto &ball : ballList)
        isBallEventProcessed |= ball.processEvent(event, window);
    if (isBallEventProcessed) return;
    // * Process key pressed
    if (const sf::Event::KeyPressed *keyPressed =
            event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::R) {
            for (auto &ball : ballList) {
                ball.setPosition(position + size / 2.f);
                ball.setPreviousPosition(position + size / 2.f);
            }
        }
        if (keyPressed->code == sf::Keyboard::Key::Q) {
            addBall(sf::Color::Red, false, 10, 30);
        }
        if (keyPressed->code == sf::Keyboard::Key::W) {
            addBall(sf::Color::Cyan, false, 1, 20);
        }
        if (keyPressed->code == sf::Keyboard::Key::E) {
            removeBall();
        }
        if (keyPressed->code == sf::Keyboard::Key::T) {
            addBall(sf::Color::Green, false, 100, 40);
        }
        if (keyPressed->code == sf::Keyboard::Key::C) {
            ballList.clear();
        }
    }
}

void BallScene::processInput() {
    for (auto &ball : ballList) ball.processInput(window);
    ballBounceSlider.processInput(window);
    wallBounceSlider.processInput(window);
    gravityStrengthSlider.processInput(window);

    for (auto &ball : ballList) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            ball.addAcceleration({0, -ACCELERATE_FORCE});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            ball.addAcceleration({-ACCELERATE_FORCE, 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            ball.addAcceleration({0, ACCELERATE_FORCE});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            ball.addAcceleration({ACCELERATE_FORCE, 0});
    }
}

int BallScene::randRange(int lowerVal, int upperVal) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(lowerVal, upperVal);
    return distribution(gen);
}

void BallScene::addBall(sf::Color color, bool isRandom, int mass, int radius) {
    sf::Vector2f ballPosition;
    if (isRandom) {
        ballPosition.x = randRange(position.x, position.x + size.x);
        ballPosition.y = randRange(position.y, position.y + size.y);
    } else {
        ballPosition = sf::Vector2f{
            window.mapPixelToCoords(sf::Mouse::getPosition(window))};
    }
    ballList.push_back(Ball(ballPosition, mass, radius, color));
}

void BallScene::resolveBallCollision() {
    for (auto it1 = ballList.begin(); it1 != ballList.end(); ++it1) {
        Ball &ball1 = *it1;
        for (auto it2 = it1 + 1; it2 != ballList.end(); ++it2) {
            Ball &ball2 = *it2;

            sf::Vector2f delta = ball1.getPosition() - ball2.getPosition();
            float distance = delta.length();
            float minDistance = ball1.getRadius() + ball2.getRadius();

            // Check if balls are colliding
            if (distance < minDistance) {
                // Normalize the collision vector
                sf::Vector2f normal;

                if (distance == 0)
                    normal = sf::Vector2f{1.f, 0.f}.rotatedBy(
                        sf::Angle(sf::degrees(randRange(0, 360))));
                else
                    normal = delta / distance;

                // Calculate the overlap and separate balls
                float overlap = minDistance - distance;
                float mass1 = ball1.getMass();
                float mass2 = ball2.getMass();
                float totalMass = mass1 + mass2;

                // Move balls apart proportional to their mass
                ball1.shift(1.f * normal * (overlap * mass2 / totalMass));
                ball2.shift(-1.f * normal * (overlap * mass1 / totalMass));

                // Calculate relative velocity
                sf::Vector2f relativeVelocity =
                    ball1.getVelocity() - ball2.getVelocity();

                // Get bounce coefficient
                float bounceFactor = ballBounceSlider.getValue();

                // Project relative velocity onto collision normal
                float velAlongNormal = relativeVelocity.dot(normal);

                // Do not resolve if objects are moving away from each other
                if (velAlongNormal <= 0) {
                    // Calculate impulse scalar
                    float impulseScalar =
                        -(1.0f + bounceFactor) * velAlongNormal;
                    impulseScalar /= (1.0f / mass1) + (1.0f / mass2);

                    // Apply impulse to velocities
                    sf::Vector2f impulse = normal * impulseScalar;
                    ball1.addVelocity(+1.0f * impulse / mass1);
                    ball2.addVelocity(-1.0f * impulse / mass2);
                }
            }
        }
    }
    // std::cerr << ball1.getVelocity().length() << " " <<
    // ball2.getVelocity().length() << "\n";
}

void BallScene::resolveBorderCollision() {
    for (Ball &ball : ballList) {
        sf::Vector2f ballPosition = ball.getPosition();
        sf::Vector2f velocity = ball.getVelocity();
        float radius = ball.getRadius();
        bool isBounced = false;
        float conservationRatio = wallBounceSlider.getValue();
        if (ballPosition.x > position.x + size.x - radius) {
            ballPosition.x = position.x + size.x - radius;
            velocity.x *= -conservationRatio;
        }
        if (ballPosition.y > position.y + size.y - radius) {
            ballPosition.y = position.y + size.y - radius;
            velocity.y *= -conservationRatio;
        }
        if (ballPosition.x < position.x + radius) {
            ballPosition.x = position.x + radius;
            velocity.x *= -conservationRatio;
        }
        if (ballPosition.y < position.y + radius) {
            ballPosition.y = position.y + radius;
            velocity.y *= -conservationRatio;
        }

        ball.setPosition(ballPosition);
        ball.setVelocity(velocity);
    }
}

void BallScene::resolveCollision() {
    int substeps = 2;
    for (int i = 0; i < substeps; i++) {
        resolveBallCollision();
        resolveBorderCollision();
    }
}

float BallScene::getEnergy() {
    float res = 0.f;
    for (auto &ball : ballList)
        res = res + 0.5f * ball.getMass() * ball.getVelocity().lengthSquared();
    return res;
}

void BallScene::removeBall() {
    sf::Vector2f mouseWorldPosition =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (auto iterator = ballList.begin(); iterator != ballList.end();
         iterator++) {
        Ball &ball = *iterator;
        if ((mouseWorldPosition - ball.getPosition()).length() <
            ball.getRadius()) {
            ballList.erase(iterator);
            return;
        }
    }
}