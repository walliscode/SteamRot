////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneResources struct.
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Resource struct containing scene-level resources.
///
/// SceneResources owns all scene-level resources. Currently this only
/// includes the render texture for the scene. EntityMemoryPool and
/// ArchetypeManager remain in EntityManager.
///
/// All members are concrete objects (no references/pointers to other
/// resource members). This struct is default-constructible and should
/// be configured via ConfigureSceneResources free function.
////////////////////////////////////////////////////////////
struct SceneResources {
  ////////////////////////////////////////////////////////////
  /// @brief Render texture for the scene.
  ///
  /// All scene rendering is done to this texture, which is then
  /// drawn to the game window.
  ////////////////////////////////////////////////////////////
  sf::RenderTexture scene_texture;
};

} // namespace steamrot
