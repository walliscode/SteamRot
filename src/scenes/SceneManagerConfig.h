/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneManagerConfig struct.
/////////////////////////////////////////////////

#pragma once

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SceneManagerConfig
/// @brief SceneManager configuration struct containing settings loaded from data.
///
/// SceneManagerConfig contains configuration loaded from data files.
/// This includes SceneManager-specific settings and preferences.
/// All configuration is loaded at initialization and is mostly read-only
/// during runtime.
///
/// This struct is created even though initially empty for consistency
/// with Engine and Scene patterns, and to provide a clear place for
/// future configuration (e.g., max concurrent scenes, transition settings, etc.).
/////////////////////////////////////////////////
struct SceneManagerConfig {
  SceneManagerConfig() = default;

  // Future: SceneManager-specific settings
  // - max concurrent scenes
  // - transition settings
  // - scene loading strategy
};

} // namespace steamrot
