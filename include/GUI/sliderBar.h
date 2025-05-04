#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <optional>
#include <vector>
class SliderBar : public sf::Drawable {
   private:
    sf::RectangleShape sliderBase;
    sf::RectangleShape sliderKnob;
    static constexpr int KNOB_HEIGHT_ADDITION = 10;
    static constexpr int KNOB_WIDTH = 20;
    bool isHolding = false;
    // * This should be between 0 (exclusive) and 1 (inclusive), with 0 means
    // the part takes no place inside the slider bar and 1 means takes the whole
    // slider bar
    std::vector<float> portionSize;
    // * Should have the size larger than portionSize exactly 1
    std::vector<float> sliderValue;

    float currentPercentage;

    sf::Vector2f getKnobPosition(float percentage);

    void updatePosition(sf::Vector2f position);

   public:
    SliderBar(sf::Vector2f position, sf::Vector2f size,
              std::vector<float> portionSize, std::vector<float> sliderValue);

    void handleEvent(sf::RenderWindow &window,
                     const std::optional<sf::Event> &event);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    void render(sf::RenderWindow &window);

    void update(sf::RenderWindow &window);

    
};