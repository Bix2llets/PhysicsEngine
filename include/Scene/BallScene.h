#pragma once
#include "Scene/Scene.h"
#include "ball.h"
#include "GUI/sliderBar.h"
#include <vector>
class BallScene : public Scene {

    public:
    BallScene(sf::RenderWindow &window, sf::Vector2f position, sf::Vector2f size);
    void render();
    void update();
    void processEvent(std::optional<sf::Event> &event);
    void processInput();
    // * Constructor and inherited pure virtual methods
    private: 

    sf::Vector2f position;
    sf::Vector2f size;
    std::vector<Ball> ballList;
    void resolveCollision();
    void resolveBallCollision();
    void resolveBorderCollision();
    SliderBar gravityStrengthSlider, ballBounceSlider, wallBounceSlider;

    static const std::vector<float> GRAVITY_STRENGTH_DIVISION;
    static const std::vector<float> GRAVITY_STRENGTH_VALUES;
    static const sf::Vector2f GRAVITY_BAR_POSITION;

    static const std::vector<float> BALL_BOUNCE_DIVISION;
    static const std::vector<float> BALL_BOUNCE_VALUES;
    static const sf::Vector2f BALL_BAR_POSITION;

    static const std::vector<float> WALL_BOUNCE_DIVISION;
    static const std::vector<float> WALL_BOUNCE_VALUES;
    static const sf::Vector2f WALL_BAR_POSITION; 

    static const sf::Vector2f BAR_DIMENSION;
    static const float ACCELERATE_FORCE;

    void addBall(sf::Color color, bool isRandom = false, int mass = 1, int radius = 20);
    void removeBall();
    int randRange(int lowerVal, int upperVal);

    public:
    float getEnergy();
};