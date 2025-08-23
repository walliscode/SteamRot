/////////////////////////////////////////////////
/// @file
/// @brief Implementation of DisplayManager class
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "DisplayManager.h"
#include "Session.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

namespace steamrot {

///////////////////////////////////////////////////////////
DisplayManager::DisplayManager(sf::RenderWindow &window)
    : m_active_session(std::make_shared<Session>()), m_window(window) {};

///////////////////////////////////////////////////////////
void DisplayManager::Render() {};

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
