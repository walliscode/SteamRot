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
  // add container/wrapper functions for drawing here

  // AND HERE
  // display the window
  m_window.display();

  return std::monostate{};
}
} // namespace steamrot
