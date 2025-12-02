/////////////////////////////////////////////////
/// @file
/// @brief File-based implementation of user preferences provider.
///
/// FlatbuffersUserPreferencesProvider loads and saves user preferences
/// using FlatBuffers binary files. It supports:
/// - Loading from user-specific preferences file
/// - Falling back to defaults from binary file if no user file exists
/// - Saving preferences to user-specific file
///
/// ## File Locations
/// - Default preferences: {data_dir}/preferences/default.preferences.bin
/// - User preferences: {data_dir}/preferences/user_preferences.bin
///
/// ## Usage
/// ```cpp
/// FlatbuffersUserPreferencesProvider provider;
/// auto prefs = provider.LoadPreferences();
/// // Modify prefs...
/// provider.SavePreferences(prefs.value());
/// ```
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IUserPreferencesProvider.h"
#include <filesystem>

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersUserPreferencesProvider
/// @brief FlatBuffers file-based user preferences provider.
///
/// This implementation:
/// - Loads default preferences from default.preferences.bin
/// - Loads user overrides from user_preferences.bin if it exists
/// - Saves user preferences to user_preferences.bin
/////////////////////////////////////////////////
class FlatbuffersUserPreferencesProvider : public IUserPreferencesProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Get the path to user preferences file.
  ///
  /// @return Path to user preferences file
  /////////////////////////////////////////////////
  std::filesystem::path GetUserPreferencesPath() const;

  /////////////////////////////////////////////////
  /// @brief Load default preferences from binary file.
  ///
  /// @return Default user preferences or failure information
  /////////////////////////////////////////////////
  std::expected<UserPreferences, FailInfo> LoadDefaultPreferences() const;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor.
  /////////////////////////////////////////////////
  FlatbuffersUserPreferencesProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load user preferences.
  ///
  /// Loads from user preferences file if it exists,
  /// otherwise returns default preferences from binary file.
  ///
  /// @return User preferences or failure information
  /////////////////////////////////////////////////
  std::expected<UserPreferences, FailInfo> LoadPreferences() const override;

  /////////////////////////////////////////////////
  /// @brief Save user preferences to file.
  ///
  /// @param preferences The preferences to save
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  SavePreferences(const UserPreferences &preferences) override;

  /////////////////////////////////////////////////
  /// @brief Check if user-specific preferences file exists.
  ///
  /// @return true if user preferences file exists
  /////////////////////////////////////////////////
  bool HasUserPreferences() const override;
};

} // namespace steamrot
