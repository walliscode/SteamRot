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
#include <variant>

namespace steamrot {

///////////////////////////////////////////////////////////
DisplayManager::DisplayManager(sf::RenderWindow &window,
                               const SceneInfoProvider &scene_manager_interface)
    : m_active_session(std::make_shared<Session>()), m_window(window),
      m_scene_manager_interface(scene_manager_interface) {};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> DisplayManager::CallRenderCycle() {
  // clear the window
  m_window.clear(sf::Color::Black);

  // ADD CODE BETWEEN HERE
  DrawTextures();
  // AND HERE

  // display the window
  m_window.display();

  return std::monostate{};
}

/////////////////////////////////////////////////
void DisplayManager::DrawTextures() {
  // get all available scene info
  auto scene_info_result =
      m_scene_manager_interface.ProvideAvailableSceneInfo();

  // generate a vector of scene ids
  std::vector<uuids::uuid> scene_ids;
  if (scene_info_result) {
    for (const auto &scene_info : scene_info_result.value()) {
      scene_ids.push_back(scene_info.id);
    }
  }

  // get textures for the scenes
  auto textures_result = m_scene_manager_interface.ProvideTextures(scene_ids);

  // draw each scene's texture to the window
  for (const auto &texture_pair : textures_result.value()) {
    const auto &render_texture = texture_pair.second.get();
    sf::Sprite sprite(render_texture.getTexture());
    m_window.draw(sprite);
  }
}
} // namespace steamrot
