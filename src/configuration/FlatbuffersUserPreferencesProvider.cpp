/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersUserPreferencesProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUserPreferencesProvider.h"
#include "FlatbuffersDataLoader.h"
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
  // If user preferences file exists, load from it (user overrides)
  if (HasUserPreferences()) {
    // TODO: Implement loading user overrides from FlatBuffers binary file
    // For now, return defaults (stub implementation)
    return LoadDefaultPreferences();
  }

  // No user preferences file, return defaults from binary file
  return LoadDefaultPreferences();
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
std::expected<UserPreferences, FailInfo>
FlatbuffersUserPreferencesProvider::LoadDefaultPreferences() const {
  // Load default preferences from FlatBuffers binary file
  FlatbuffersDataLoader data_loader;

  auto prefs_data_result = data_loader.ProvideDefaultUserPreferencesData();
  if (!prefs_data_result.has_value()) {
    return std::unexpected(prefs_data_result.error());
  }

  const UserPreferencesData *prefs_data = prefs_data_result.value();

  // Convert FlatBuffers data to UserPreferences struct
  UserPreferences preferences;

  // Display preferences
  if (prefs_data->display()) {
    preferences.display.window_width = prefs_data->display()->window_width();
    preferences.display.window_height = prefs_data->display()->window_height();
    preferences.display.fullscreen = prefs_data->display()->fullscreen();
    preferences.display.vsync = prefs_data->display()->vsync();
    preferences.display.target_framerate =
        prefs_data->display()->target_framerate();
  }

  // Audio preferences
  if (prefs_data->audio()) {
    preferences.audio.master_volume = prefs_data->audio()->master_volume();
    preferences.audio.music_volume = prefs_data->audio()->music_volume();
    preferences.audio.sfx_volume = prefs_data->audio()->sfx_volume();
    preferences.audio.muted = prefs_data->audio()->muted();
  }

  // Accessibility preferences
  if (prefs_data->accessibility()) {
    preferences.accessibility.ui_scale =
        prefs_data->accessibility()->ui_scale();
    if (prefs_data->accessibility()->preferred_font()) {
      preferences.accessibility.preferred_font =
          prefs_data->accessibility()->preferred_font()->str();
    }
  }

  preferences.version = prefs_data->version();

  return preferences;
}

} // namespace steamrot
