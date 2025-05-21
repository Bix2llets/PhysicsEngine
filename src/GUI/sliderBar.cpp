#include "GUI/sliderBar.h"

#include <algorithm>
#include <iostream>
SliderBar::SliderBar(sf::Vector2f position, sf::Vector2f size,
                     std::vector<float> portionSize,
                     std::vector<float> sliderValue)
    : portionSize{portionSize},
      sliderValue{sliderValue},
      textFont{"assets/Inter-Regular.ttf"},
      sliderText{textFont} {
    sliderText.setFillColor(sf::Color::White);

    sliderBase.setPosition(position);
    sliderBase.setSize(size);

    sf::Vector2f textPosition;
    textPosition = sliderBase.getPosition();
    textPosition.x += sliderBase.getSize().x / 2;
    textPosition.y += sliderBase.getSize().y + KNOB_HEIGHT_ADDITION + 10;
    sliderText.setPosition(textPosition);

    currentPercentage = 0.5f;

    std::string text = std::to_string(getValue());
    text = text.substr(0, text.find('.') + 1 + 2);
    sliderText.setString(text);
    sf::FloatRect bound = sliderText.getLocalBounds();
    sliderText.setOrigin(bound.size / 2.0f);

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
    target.draw(sliderText);
}

bool SliderBar::processEvent(std::optional<sf::Event> &event,
                             sf::RenderWindow &window) {
    bool result = false;
    result |= processLeftMouseClicked(event, window);
    result |= processLeftMouseReleased(event, window);
    return result;
}

bool SliderBar::processLeftMouseClicked(std::optional<sf::Event> &event,
                                        sf::RenderWindow &window) {
    if (isHeldLeft) return false;
    auto *mouseEventPressed = event->getIf<sf::Event::MouseButtonPressed>();

    if (mouseEventPressed == nullptr) return false;
    if (mouseEventPressed->button != sf::Mouse::Button::Left) return false;
    sf::Vector2f mousePosition =
        window.mapPixelToCoords(mouseEventPressed->position);
    if (!sliderBase.getGlobalBounds().contains(mousePosition) &&
        !sliderKnob.getGlobalBounds().contains(mousePosition))
        return false;
    isHeldLeft = true;
    sf::Vector2f newPosition =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));
    updatePosition(newPosition);
    return true;
}

bool SliderBar::processLeftMouseReleased(std::optional<sf::Event> &event,
                                         sf::RenderWindow &window) {
    if (!isHeldLeft) return false;
    const auto *mouseEventReleased =
        event->getIf<sf::Event::MouseButtonReleased>();
    if (mouseEventReleased == nullptr) return false;
    if (mouseEventReleased->button != sf::Mouse::Button::Left) return false;
    isHeldLeft = false;
    return true;
}

void SliderBar::update() {
    if (!isHeldLeft) return;
    std::string text = std::to_string(getValue());
    int decimalLocation = text.find('.', 0);
    text = text.substr(0, decimalLocation + 3);
    sliderText.setString(text);
    sf::FloatRect bound = sliderText.getLocalBounds();
    sliderText.setOrigin(bound.size / 2.0f);
    // sliderText.setPosition(sliderKnob.getPosition() + sf::Vector2f{0, 20});
}

void SliderBar::updatePosition(sf::Vector2f position) {
    float newPercentage =
        (position.x - sliderBase.getPosition().x) / sliderBase.getSize().x;

    currentPercentage = newPercentage;
    currentPercentage = std::max(currentPercentage, 0.f);
    currentPercentage = std::min(currentPercentage, 1.f);
    sliderKnob.setPosition(getKnobPosition(currentPercentage));
}

float SliderBar::getValue() {
    std::vector<float> prefix;
    prefix.resize(portionSize.size() + 1);
    prefix[0] = 0.f;
    for (int i = 1; i < prefix.size(); i++)
        prefix[i] = portionSize[i - 1] + prefix[i - 1];

    auto upperBoundResult =
        std::lower_bound(prefix.begin(), prefix.end(), currentPercentage);
    // assert(upperBoundResult != prefix.end());
    int valueIndex = upperBoundResult - prefix.begin() - 1;

    float percentageDifference = prefix[valueIndex + 1] - prefix[valueIndex];
    float ratio =
        (currentPercentage - prefix[valueIndex]) / (percentageDifference);

    float valueDifference =
        sliderValue[valueIndex + 1] - sliderValue[valueIndex];
    // std::cerr << currentPercentage << " " << ratio * valueDifference << "\n";
    return sliderValue[valueIndex] + ratio * valueDifference;
}

void SliderBar::processInput(sf::RenderWindow &window) {
    if (isHeldLeft) {
        sf::Vector2f mousePosition =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));
        updatePosition(mousePosition);
    }
}