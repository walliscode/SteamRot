/////////////////////////////////////////////////
/// @file
/// @brief Quick sfml window for testing whatever
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaFormScaffold.h"
#include "paths.h"
#include "render_grimoire_machina.h"
#include <SFML/Graphics.hpp>
#include <string>

int main() {

  // set up resources
  sf::Font font;
  std::filesystem::path font_path =
      steamrot::paths::GetDataDirectory() /
      std::filesystem::path{"assets/fonts/Roboto-Regular.ttf"};

  if (!font.openFromFile(font_path.string()))
    return 1;

  sf::FloatRect box{{100.f, 100.f}, {400.f, 300.f}};
  sf::Color color{sf::Color::Red};
  std::string text{"Analysis not run"};
  sf::RenderTexture texture{{800, 600}};

  sf::RenderWindow window(sf::VideoMode({800, 600}), "Quick Test Window");
  window.setFramerateLimit(60);
  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      // "close requested" event: we close the window
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    window.clear(sf::Color::Black);
    texture.clear(sf::Color::Black);
    /// CODE ///

    steamrot::logic::render::grimoire_machina::pick_and_draw_status_box(
        steamrot::StructuralAnalysisState::NothingFound, box, font, texture);

    /// END CODE ///
    texture.display();
    sf::Sprite sprite(texture.getTexture());
    window.draw(sprite);
    window.display();
  }
  return 0;
}
