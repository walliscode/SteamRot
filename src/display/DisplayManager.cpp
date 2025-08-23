/////////////////////////////////////////////////
/// @file
/// @brief Implementation of DisplayManager class
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "DisplayManager.h"
#include "Session.h"
#include "TexturesPackage.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

namespace steamrot {

///////////////////////////////////////////////////////////
DisplayManager::DisplayManager(sf::RenderWindow &window)
    : m_active_session(std::make_shared<Session>()), m_window(window) {};

///////////////////////////////////////////////////////////
void DisplayManager::Render(TexturesPackage &textures_package) {
  // clear the window with the background color, this always be at the start
  m_window.clear();

  // get tiles from active session
  if (!m_active_session) {
    throw std::runtime_error("No active session set for DisplayManager");
  }
  auto tiles = m_active_session->GetTiles();

  // get the tile overlay texture from the textures package
  for (auto &tile : tiles) {

    // // print tile scene ID for debugging
    // std::cout << "Tile scene ID: " << tile->GetSceneId() << std::endl;
    // check if held scene id is in the textures package
    auto scene_texture =
        textures_package.GetTextures().find(tile->GetSceneId());

    // if yes, then draw the scene texture to that tile
    if (scene_texture != textures_package.GetTextures().end()) {

      // create sprite from tile texture, this is a reference wrapper so .get()
      // method is necessary
      sf::Sprite tile_sprite{scene_texture->second.get().getTexture()};

      // draw the sprite to the window
      m_window.draw(tile_sprite);
    }
  }

  // display the contents of the window to the screen
  m_window.display();
};

void DisplayManager::LoadTitleSceneTiles(const uuids::uuid &title_scene_id) {
  std::cout << "DisplayManager: Loading title scene tiles with ID: "
            << title_scene_id << std::endl;
  for (auto &session : m_sessions) {
    session = std::make_shared<Session>();
  }
  // set the active session to the first session in the array
  m_active_session = m_sessions[0];

  // pass the title scene ID to the active tile in the active session
  m_active_session->GetActiveTile()->SetSceneId(title_scene_id);
};

void DisplayManager::LoadCraftingSceneTiles(
    const uuids::uuid &crafting_scene_id) {
  std::cout << "DisplayManager: Loading crafting scene tiles with ID: "
            << crafting_scene_id << std::endl;
  // create a new session for the crafting scene
  auto crafting_session = std::make_shared<Session>();
  // set the active session to the crafting session
  m_active_session = crafting_session;
  // pass the crafting scene ID to the active tile in the active session
  m_active_session->GetActiveTile()->SetSceneId(crafting_scene_id);
};

} // namespace steamrot
