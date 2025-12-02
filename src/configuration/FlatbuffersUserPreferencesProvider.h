/////////////////////////////////////////////////
/// @file
/// @brief File-based implementation of user preferences provider.
///
/// FlatbuffersUserPreferencesProvider loads and saves user preferences
/// using FlatBuffers binary files. It supports:
/// - Loading from user-specific preferences file
/// - Falling back to defaults if no user file exists
/// - Saving preferences to user-specific file
///
/// ## File Locations
/// - Default preferences: Built into the application
/// - User preferences: {user_data_dir}/preferences/user_preferences.bin
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
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersUserPreferencesProvider
/// @brief FlatBuffers file-based user preferences provider.
///
/// This implementation:
/// - Loads user preferences from binary FlatBuffers files
/// - Returns defaults if no user file exists
/// - Saves preferences to user-specific location
/////////////////////////////////////////////////
class FlatbuffersUserPreferencesProvider : public IUserPreferencesProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Get the path to user preferences file.
  ///
  /// @return Path to user preferences file
  /////////////////////////////////////////////////
  std::filesystem::path GetUserPreferencesPath() const;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor.
  /////////////////////////////////////////////////
  FlatbuffersUserPreferencesProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load user preferences.
  ///
  /// Loads from user preferences file if it exists,
  /// otherwise returns default preferences.
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

  /////////////////////////////////////////////////
  /// @brief Get default preferences.
  ///
  /// @return Default user preferences
  /////////////////////////////////////////////////
  UserPreferences GetDefaultPreferences() const override;
};

} // namespace steamrot
