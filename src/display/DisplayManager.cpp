////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "DisplayManager.h"
#include "Session.h"

////////////////////////////////////////////////////////////
// namespaces/using
////////////////////////////////////////////////////////////
namespace steamrot {

///////////////////////////////////////////////////////////
DisplayManager::DisplayManager()
    : m_active_session(std::make_shared<Session>()) {};

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
void DisplayManager::Render(TexturesPackage &textures_package) {

  // clear the window with the background color, this always be at the start
  m_window.clear(sf::Color::Magenta);

  // get tiles from active session
  if (!m_active_session) {
    throw std::runtime_error("No active session set for DisplayManager");
  }
  auto tiles = m_active_session->GetTiles();
  //
  // get the tile overlay texture from the textures package
  for (auto &tile : tiles) {

    // check if id is in the textures package
    auto tile_texture = textures_package.GetTextures().find(tile->GetSceneId());

    if (tile_texture != textures_package.GetTextures().end()) {

      // create sprite from tile texture
      sf::Sprite tile_sprite{tile_texture->second->getTexture()};

      // draw the sprite to the window
      m_window.draw(tile_sprite);
    }
  }
  // tile overlay should come last
  // create sprite from tile overlay texture
  sf::Sprite tile_overlay_sprite{m_tile_overlay.getTexture()};
  m_window.draw(tile_overlay_sprite);

  // display the contents of the window to the screen
  m_window.display();
};

///////////////////////////////////////////////////////////
void DisplayManager::Update() {

};
} // namespace steamrot
