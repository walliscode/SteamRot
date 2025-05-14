////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "DisplayManager.h"

////////////////////////////////////////////////////////////
// namespaces/using
////////////////////////////////////////////////////////////
namespace steamrot {

///////////////////////////////////////////////////////////
DisplayManager::DisplayManager() {

};

///////////////////////////////////////////////////////////
void DisplayManager::SetWindowConfig(const json &config) {
  // cast json values to to the correct types, to show intent
  sf::Vector2u size{config["width"], config["height"]};
  m_window.setSize(size);

  const std::string title = config["title"];
  m_window.setTitle(title);

  unsigned int frame_rate_limit = config["framerate_limit"];
  m_window.setFramerateLimit(frame_rate_limit);

  m_background_color.r = config["background_color"]["r"];
  m_background_color.g = config["background_color"]["g"];
  m_background_color.b = config["background_color"]["b"];
};

///////////////////////////////////////////////////////////
sf::RenderWindow &DisplayManager::GetWindow() { return m_window; };

///////////////////////////////////////////////////////////
void DisplayManager::Render() {

  // clear the window with the background color, this always be at the start
  m_window.clear();

  // TODO: pass through RenderTextures from the Scenes and draw

  // tile overlay should come last
  // create sprite from tile overlay texture
  sf::Sprite tile_overlay_sprite{m_tile_overlay.getTexture()};
  m_window.draw(tile_overlay_sprite);

  // display the contents of the window to the screen
  m_window.display();
};
} // namespace steamrot
