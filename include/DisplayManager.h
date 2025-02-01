#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class DisplayManager {

public:
  DisplayManager();
  void SetWindowConfig(const json &config);
  sf::RenderWindow &GetWindow();

  void Cycle();

private:
  // add some defaults to the window otherwise it doens't behave itself
  // may have initialise DisplayManager with a json object so i can pass
  // constructors into the window
  sf::RenderWindow m_window{sf::VideoMode({800, 600}), "SFML window"};
  sf::Color m_background_color;

  // std::array<Session, 5> m_sessions;
};
