/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersUserPreferencesProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUserPreferencesProvider.h"
#include "paths.h"

namespace steamrot {

/////////////////////////////////////////////////
std::filesystem::path
FlatbuffersUserPreferencesProvider::GetUserPreferencesPath() const {
  // Use the preferences directory path helper
  return paths::GetPreferencesDirectory() / "user_preferences.bin";
}

/////////////////////////////////////////////////
std::expected<UserPreferences, FailInfo>
FlatbuffersUserPreferencesProvider::LoadPreferences() const {
  // If user preferences file exists, load from it
  if (HasUserPreferences()) {
    // TODO: Implement loading from FlatBuffers binary file
    // For now, return defaults (stub implementation)
    return GetDefaultPreferences();
  }

  // No user preferences file, return defaults
  return GetDefaultPreferences();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUserPreferencesProvider::SavePreferences(
    const UserPreferences &preferences) {
  // TODO: Implement saving to FlatBuffers binary file
  // For now, this is a stub implementation
  // In a full implementation:
  // 1. Create the preferences directory if it doesn't exist
  // 2. Build a FlatBuffers UserPreferencesData object
  // 3. Write to binary file

  return std::monostate{};
}

/////////////////////////////////////////////////
bool FlatbuffersUserPreferencesProvider::HasUserPreferences() const {
  return std::filesystem::exists(GetUserPreferencesPath());
}

/////////////////////////////////////////////////
UserPreferences FlatbuffersUserPreferencesProvider::GetDefaultPreferences() const {
  // Return built-in defaults
  UserPreferences defaults;

  // Display defaults
  defaults.display.window_width = 0; // 0 = use context_data default
  defaults.display.window_height = 0;
  defaults.display.fullscreen = false;
  defaults.display.vsync = true;
  defaults.display.target_framerate = 60;

  // Audio defaults
  defaults.audio.master_volume = 1.0f;
  defaults.audio.music_volume = 0.8f;
  defaults.audio.sfx_volume = 1.0f;
  defaults.audio.muted = false;

  // Accessibility defaults
  defaults.accessibility.ui_scale = 1.0f;
  defaults.accessibility.preferred_font = ""; // Empty = use default font

  defaults.version = 1;

  return defaults;
}

} // namespace steamrot
