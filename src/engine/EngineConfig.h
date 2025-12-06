/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the EngineConfig struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <cstdint>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Display configuration settings.
/////////////////////////////////////////////////
struct DisplayConfig {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
  bool fullscreen{false};
  bool vsync{true};
};

/////////////////////////////////////////////////
/// @brief User preferences configuration.
/////////////////////////////////////////////////
struct UserPreferencesConfig {
  float master_volume{1.0f};
  bool show_fps{false};
  std::string preferred_language{"en"};
};

/////////////////////////////////////////////////
/// @brief Engine configuration struct containing all settings.
///
/// EngineConfig contains all configuration loaded from data files.
/// This includes display settings, user preferences, and other
/// engine-level configuration. All configuration is loaded at startup
/// and is mostly read-only during runtime.
/////////////////////////////////////////////////
struct EngineConfig {
  DisplayConfig display;
  UserPreferencesConfig user_preferences;
};

} // namespace steamrot
