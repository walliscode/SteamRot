/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneConfig struct.
/////////////////////////////////////////////////

#pragma once

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SceneConfig
/// @brief Scene configuration struct containing settings loaded from data.
///
/// SceneConfig contains configuration loaded from scene data files.
/// This includes scene-specific settings and preferences.
/// All configuration is loaded at scene creation and is mostly read-only
/// during runtime.
///
/// Note: Event handling is done via Subscribers, not stored as a config set.
/// This struct may be empty initially but is created for consistency and
/// future extensibility (render settings, gameplay parameters, AI difficulty,
/// etc.).
/////////////////////////////////////////////////
struct SceneConfig {
  SceneConfig() = default;

  // Future: scene-specific settings
  // - render settings
  // - gameplay parameters
  // - AI difficulty
};

} // namespace steamrot
