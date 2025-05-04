#include "GUI/sliderBar.h"
#include <algorithm>
SliderBar::SliderBar(sf::Vector2f position, sf::Vector2f size,
                     std::vector<float> portionSize,
                     std::vector<float> sliderValue)
    : portionSize{portionSize}, sliderValue{sliderValue} {
    sliderBase.setPosition(position);
    sliderBase.setSize(size);

    currentPercentage = 0.5f;

    sliderKnob.setSize(
        {KNOB_WIDTH, KNOB_HEIGHT_ADDITION * 2 + sliderBase.getSize().y});
    sliderKnob.setOrigin(sliderKnob.getSize() / 2.0f);
    sliderKnob.setFillColor(sf::Color::Red);
    sliderKnob.setPosition(getKnobPosition(currentPercentage));
}

sf::Vector2f SliderBar::getKnobPosition(float percentage) {
    // Assuming the origin is at the center of the slider knob
    using V2f = sf::Vector2f;
    V2f handlePosition;
    handlePosition.y = sliderBase.getPosition().y + sliderBase.getSize().y / 2;
    handlePosition.x =
        sliderBase.getPosition().x + sliderBase.getSize().x * percentage;
    return handlePosition;
}
void SliderBar::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(sliderBase, states);
    target.draw(sliderKnob, states);
}

void SliderBar::handleEvent(sf::RenderWindow &window,
                            const std::optional<sf::Event> &event) {
    const auto *mouseEventPressed =
        event->getIf<sf::Event::MouseButtonPressed>();

    if (mouseEventPressed != nullptr &&
        mouseEventPressed->button == sf::Mouse::Button::Left) {
        sf::Vector2f mousePosition = sf::Vector2f(mouseEventPressed->position);
        isHolding = true;
        sf::Vector2f newPosition =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));
        updatePosition(newPosition);
    }

    const auto *mouseEventReleased =
        event->getIf<sf::Event::MouseButtonReleased>();

    if (mouseEventReleased != nullptr &&
        mouseEventReleased->button == sf::Mouse::Button::Left)
        isHolding = false;
}

void SliderBar::render(sf::RenderWindow &window) { window.draw(*this); }

void SliderBar::update(sf::RenderWindow &window) {
    if (!isHolding) return;

    sf::Vector2f newPosition =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    updatePosition(newPosition);
}

void SliderBar::updatePosition(sf::Vector2f position) {
    float newPercentage =
        (position.x - sliderBase.getPosition().x) / sliderBase.getSize().x;

    currentPercentage = newPercentage;
    currentPercentage = std::max(currentPercentage, 0.f);
    currentPercentage = std::min(currentPercentage, 1.f);
    sliderKnob.setPosition(getKnobPosition(currentPercentage));

}