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

namespace steamrot {

///////////////////////////////////////////////////////////
DisplayManager::DisplayManager(sf::RenderWindow &window,
                               const SceneInfoProvider &scene_manager_interface)
    : m_active_session(std::make_shared<Session>()), m_window(window),
      m_scene_manager_interface(scene_manager_interface) {};

} // namespace steamrot
