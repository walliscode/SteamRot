/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneManagerResources struct.
/////////////////////////////////////////////////

#pragma once

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SceneManagerResources
/// @brief SceneManager-level resources struct containing long-lived objects.
///
/// SceneManagerResources owns all SceneManager-level resources that have
/// a lifetime matching the SceneManager instance.
///
/// Note: Per user request, m_scenes map is kept at the top level in
/// SceneManager (not nested in this struct) to make it obvious.
///
/// This struct is created even though initially empty for consistency
/// with Engine and Scene patterns, and to provide a clear place for
/// future resources (e.g., scene factory, transition manager, etc.).
/////////////////////////////////////////////////
struct SceneManagerResources {
  SceneManagerResources() = default;

  // Future: scene-level resources
  // - SceneFactory instance
  // - Transition manager
  // - Scene cache
};

} // namespace steamrot
