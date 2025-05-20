#include "Scene/BallScene.h"

#include <chrono>
#include <random>
#include <algorithm>
#include "Scene/scene.h"
const std::vector<float> BallScene::GRAVITY_STRENGTH_DIVISION = {0.4f, 0.1f,
                                                                 0.1f, 0.4f};
const std::vector<float> BallScene::GRAVITY_STRENGTH_VALUES = {
    -9800.f, -980.f, 0.f, 980.f, 9800.f};
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
    for (auto &ball : ballList) ball.update();
    gravityStrengthSlider.update();
    ballBounceSlider.update();
    wallBounceSlider.update();

    int iterations = std::min(int(1 + ballList.size() / 10), 5);
    for (int i = 0; i < iterations; i++) resolveCollision();
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
                ball.setVelocity(sf::Vector2f{0.f, 0.f});
            }
        }
        if (keyPressed->code == sf::Keyboard::Key::Q) {
            addBall(sf::Color::Red, true);
        }
        if (keyPressed->code == sf::Keyboard::Key::W) {
            addBall(sf::Color::Cyan, false);
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
            ball.accelerate({0, -ACCELERATE_FORCE});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            ball.accelerate({-ACCELERATE_FORCE, 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            ball.accelerate({0, ACCELERATE_FORCE});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            ball.accelerate({ACCELERATE_FORCE, 0});
    }
}

int BallScene::randRange(int lowerVal, int upperVal) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(lowerVal, upperVal);
    return distribution(gen);
}

void BallScene::addBall(sf::Color color, bool isRandom) {
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
                normalVector = sf::Vector2f{1, 0}.rotatedBy(sf::degrees((float)randRange(0, 360)));
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
            // assert(-1.f <= ballCollisionConservationRatio &&
            // BOUNCE_FACTOR <= 1.f);
            float speedChange = collisionSpeed * (ballBounceSlider.getValue());
            V2f deltaVelocity1 = -speedChange * normalVector;
            ball1.setVelocity(ball1.getVelocity() + deltaVelocity1);

            V2f impulseBall2 = deltaVelocity1 * ball1.getMass() * -1.f;
            V2f deltaVelocity2 = impulseBall2 / ball2.getMass();
            ball2.setVelocity(ball2.getVelocity() + deltaVelocity2);
            continue;
        }
}

void BallScene::resolveBorderCollision() {
    for (Ball &ball : ballList) {
        sf::Vector2f ballPosition = ball.getPosition();
        sf::Vector2f velocity = ball.getVelocity();
        float radius = ball.getRadius();
        if (ballPosition.x > position.x + size.x - radius) {
            ballPosition.x = position.x + size.x - radius;
            velocity.x *= -wallBounceSlider.getValue();
        }
        if (ballPosition.y > position.y + size.y - radius) {
            ballPosition.y = position.y + size.y - radius;
            velocity.y *= -wallBounceSlider.getValue();
        }
        if (ballPosition.x < position.x + radius) {
            ballPosition.x = position.x + radius;
            velocity.x *= -wallBounceSlider.getValue();
        }
        if (ballPosition.y < position.y + radius) {
            ballPosition.y = position.y + radius;
            velocity.y *= -wallBounceSlider.getValue();
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