/////////////////////////////////////////////////
/// @file
/// @brief Interface for user preferences provider.
///
/// UserPreferencesProvider defines the contract for loading and saving
/// user-level preferences. Implementations may load from files, databases,
/// or provide defaults.
///
/// ## Design Rationale
///
/// User preferences are separate from:
/// - Engine configuration (GameResourcesData - window size, framerate)
/// - Gameplay data (save files with entity states)
///
/// User preferences contain runtime-adjustable settings:
/// - Display: fullscreen, vsync (NOT window size - that's in GameResourcesData)
/// - Audio: volume levels, mute states
/// - Accessibility: UI scale, font preferences
///
/// ## Usage
/// ```cpp
/// UserPreferencesProvider provider;
/// auto prefs_result = provider.LoadPreferences();
/// if (prefs_result.has_value()) {
///   const UserPreferences& prefs = prefs_result.value();
///   // Apply preferences
/// }
/// ```
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include <expected>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct UserPreferences
/// @brief Runtime representation of user preferences.
///
/// This is the C++ representation of UserPreferencesData from FlatBuffers.
/// It provides a convenient, mutable structure for working with preferences
/// at runtime.
///
/// Note: Window size and framerate are in GameResourcesData, not here.
/// User preferences only contain runtime-adjustable settings.
/////////////////////////////////////////////////
struct UserPreferences {
  /////////////////////////////////////////////////
  /// @brief Display preferences (runtime-adjustable only)
  /// Note: Window size is in GameResourcesData, not here
  /////////////////////////////////////////////////
  struct Display {
    bool fullscreen{false};
    bool vsync{true};
  } display;

  /////////////////////////////////////////////////
  /// @brief Audio preferences
  /////////////////////////////////////////////////
  struct Audio {
    float master_volume{1.0f};
    float music_volume{0.8f};
    float sfx_volume{1.0f};
    bool muted{false};
  } audio;

  /////////////////////////////////////////////////
  /// @brief Accessibility preferences
  /////////////////////////////////////////////////
  struct Accessibility {
    float ui_scale{1.0f};
    std::string preferred_font;
  } accessibility;

  /////////////////////////////////////////////////
  /// @brief Version number for migration support
  /////////////////////////////////////////////////
  uint32_t version{1};
};

/////////////////////////////////////////////////
/// @class IUserPreferencesProvider
/// @brief Interface for loading and saving user preferences.
///
/// Implementations handle the actual storage mechanism (files, databases, etc.)
/// The interface supports:
/// - Loading preferences (returns defaults from file if no user overrides)
/// - Saving preferences to persistent storage
/// - Checking if user preferences exist
/////////////////////////////////////////////////
class IUserPreferencesProvider {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup.
  /////////////////////////////////////////////////
  virtual ~IUserPreferencesProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load user preferences.
  ///
  /// If no user preferences file exists, returns default preferences
  /// from the default preferences binary file.
  ///
  /// @return User preferences or failure information
  /////////////////////////////////////////////////
  virtual std::expected<UserPreferences, FailInfo> LoadPreferences() const = 0;

  /////////////////////////////////////////////////
  /// @brief Save user preferences to persistent storage.
  ///
  /// @param preferences The preferences to save
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  SavePreferences(const UserPreferences &preferences) = 0;

  /////////////////////////////////////////////////
  /// @brief Check if user-specific preferences exist.
  ///
  /// @return true if user preferences file exists, false otherwise
  /////////////////////////////////////////////////
  virtual bool HasUserPreferences() const = 0;
};

} // namespace steamrot
