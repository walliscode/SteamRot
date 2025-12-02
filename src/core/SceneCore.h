/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneCore struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Core struct containing scene-level core objects.
///
/// SceneCore owns all scene-level core objects. Currently this only
/// includes the render texture for the scene. EntityMemoryPool and
/// ArchetypeManager remain in EntityManager.
///
/// All members are concrete objects (no references/pointers to other
/// core members). This struct is default-constructible and should
/// be configured via ConfigureSceneCore free function.
/////////////////////////////////////////////////
struct SceneCore {
/////////////////////////////////////////////////
  /// @brief Render texture for the scene.
  ///
  /// All scene rendering is done to this texture, which is then
  /// drawn to the game window.
/////////////////////////////////////////////////
  sf::RenderTexture scene_texture;
};

} // namespace steamrot
