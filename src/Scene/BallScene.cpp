#include "Scene/BallScene.h"

#include <algorithm>
#include <chrono>
#include <random>

#include "Scene/scene.h"
#include "info.h"
const std::vector<float> BallScene::GRAVITY_STRENGTH_DIVISION = {1.f};
const std::vector<float> BallScene::GRAVITY_STRENGTH_VALUES = {0.f, 980.f};
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

    for (int i = 0; i < Info::POLLING_INTERVAL / Info::SIMULATION_INTERVAL; i++)
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
            addBall(sf::Color::Red, false, 100);
        }
        if (keyPressed->code == sf::Keyboard::Key::W) {
            addBall(sf::Color::Cyan, false, 1);
        }
        if (keyPressed->code == sf::Keyboard::Key::E) {
            removeBall();
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
            ball.addForceImpact({0, -ACCELERATE_FORCE});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            ball.addForceImpact({-ACCELERATE_FORCE, 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            ball.addForceImpact({0, ACCELERATE_FORCE});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            ball.addForceImpact({ACCELERATE_FORCE, 0});
    }
}

int BallScene::randRange(int lowerVal, int upperVal) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(lowerVal, upperVal);
    return distribution(gen);
}

void BallScene::addBall(sf::Color color, bool isRandom, int mass) {
    sf::Vector2f ballPosition;
    if (isRandom) {
        ballPosition.x = randRange(position.x, position.x + size.x);
        ballPosition.y = randRange(position.y, position.y + size.y);
    } else {
        ballPosition = sf::Vector2f{
            window.mapPixelToCoords(sf::Mouse::getPosition(window))};
    }
    ballList.push_back(
        Ball(ballPosition, 10.f, 1.f, color, sf::Color::Green, 0.f));
}

void BallScene::resolveBallCollision() {
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
                normalVector = sf::Vector2f{1, 0}.rotatedBy(
                    sf::degrees((float)randRange(0, 360)));
            else
                normalVector = positionDifference.normalized();
            float mass1 = ball1.getMass();
            float mass2 = ball2.getMass();
            // Positioning balls to remove sinking
            ball1.move(-1.0f * normalVector * overlapped *
                       (mass1 / (mass1 + mass2)));
            ball2.move(1.0f * normalVector * overlapped *
                       (mass2 / (mass1 + mass2)));

            sf::Vector2f vBall1 = ball1.getVelocity();
            sf::Vector2f vBall2 = ball2.getVelocity();

            float v1Normal = vBall1.dot(normalVector);
            float v2Normal = vBall2.dot(normalVector);

            float bounceFactor = ballBounceSlider.getValue();

            if (v1Normal > v2Normal) {
                // Calculate new velocities
                float totalMass = mass1 + mass2;
                float v1Final = ((mass1 - bounceFactor * mass2) * v1Normal +
                                 (1 + bounceFactor) * mass2 * v2Normal) /
                                totalMass;
                float v2Final = ((1 + bounceFactor) * mass1 * v1Normal +
                                 (mass2 - bounceFactor * mass1) * v2Normal) /
                                totalMass;

                // For Verlet integration, we need to set proper previous positions
                // to achieve the desired post-collision velocity
                sf::Vector2f currentPos1 = ball1.getPosition();
                sf::Vector2f currentPos2 = ball2.getPosition();

                // We need the full velocity vectors (normal + tangential components)
                sf::Vector2f tangent(-normalVector.y, normalVector.x);
                float v1Tangent = vBall1.dot(tangent);
                float v2Tangent = vBall2.dot(tangent);

                // Construct full post-collision velocities
                sf::Vector2f v1New = normalVector * v1Final + tangent * v1Tangent;
                sf::Vector2f v2New = normalVector * v2Final + tangent * v2Tangent;

                // Set previous positions to create these velocities in next update
                ball1.setPreviousPosition(currentPos1 - v1New * Info::POLLING_INTERVAL);
                ball2.setPreviousPosition(currentPos2 - v2New * Info::POLLING_INTERVAL);
            }
        }
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
    resolveBallCollision();
    resolveBorderCollision();
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