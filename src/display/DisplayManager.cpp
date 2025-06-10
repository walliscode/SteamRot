////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "DisplayManager.h"
#include "Session.h"
#include "TexturesPackage.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>

////////////////////////////////////////////////////////////
// namespaces/using
////////////////////////////////////////////////////////////
namespace steamrot {

///////////////////////////////////////////////////////////
DisplayManager::DisplayManager()
    : m_active_session(std::make_shared<Session>()), m_data_manager(),
      m_ui_engine() {

  // Initialize the RenderTexture for the UI layer wihth the same size as the
  // window
  bool resize_succesful = m_ui_layer.resize(m_window.getSize());
  // guard against resize failure
  if (!resize_succesful) {
    throw std::runtime_error("Failed to resize UI layer RenderTexture");
  }

  // Load in the desired theme data for the UI engine
  const themes::UIObjects *theme_data =
      m_data_manager.ProvideThemeData("default");

  // create the UI engine with the theme data
  m_ui_engine = UIRenderLogic(theme_data);
};

///////////////////////////////////////////////////////////
sf::RenderWindow &DisplayManager::GetWindow() { return m_window; };

///////////////////////////////////////////////////////////
void DisplayManager::Render(TexturesPackage &textures_package) {

  // clear the window with the background color, this always be at the start
  m_window.clear();

  // get tiles from active session
  if (!m_active_session) {
    throw std::runtime_error("No active session set for DisplayManager");
  }
  auto tiles = m_active_session->GetTiles();
  //
  // get the tile overlay texture from the textures package
  for (auto &tile : tiles) {

    // check if held scene id is in the textures package
    auto scene_texture =
        textures_package.GetTextures().find(tile->GetSceneId());

    // if yes, then draw the scene texture to that tile
    if (scene_texture != textures_package.GetTextures().end()) {

      // create sprite from tile texture
      sf::Sprite tile_sprite{scene_texture->second->getTexture()};

      // draw the sprite to the window
      m_window.draw(tile_sprite);
    }
  }

  // create sprite from tile overlay texture
  sf::Sprite tile_overlay_sprite{m_tile_overlay.getTexture()};
  m_window.draw(tile_overlay_sprite);

  // draw the UI layer to the window
  m_ui_engine.Draw(m_ui_layer);
  sf::Sprite ui_layer_sprite{m_ui_layer.getTexture()};
  m_window.draw(ui_layer_sprite);

  // display the contents of the window to the screen
  m_window.display();
};

///////////////////////////////////////////////////////////
void DisplayManager::Update() {}

void DisplayManager::LoadTitleSceneTiles(const uuids::uuid &title_scene_id) {
  // reset m_sessions array with fresh Session objects
  for (auto &session : m_sessions) {
    session = std::make_shared<Session>();
  }
  // set the active session to the first session in the array
  m_active_session = m_sessions[0];

  // pass the title scene ID to the active tile in the active session
  m_active_session->GetActiveTile()->SetSceneId(title_scene_id);

  // set the title scene active flag to true
  m_title_scene_active = true;
};
} // namespace steamrot
