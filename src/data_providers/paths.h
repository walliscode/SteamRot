/////////////////////////////////////////////////
/// @file
/// @brief Namespace-based path provider using compile-time environment
/// selection.
///
/// Environment Selection (compile-time macros):
/// - Default: Test environment (tests/data)
/// - STEAMROT_ENV_DEBUG: Debug environment (data) - same as production
/// - STEAMROT_ENV_PROD: Production environment (data)
///
/// The environment is controlled via the environment_config interface library
/// in src/CMakeLists.txt, which defines STEAMROT_ENV_DEBUG for production builds.
/// Tests do not link to environment_config and default to tests/data.
///
/// Directory Structure:
/// - defaults/: Default configuration data (read-only)
/// - user/: User-specific data and saves (read-write)
///
/// Usage:
///   // For production code: link to environment_config target
///   // For test code: do not link to environment_config
///   #include "paths.h"
///   auto path = steamrot::paths::GetDataDirectory();
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <filesystem>

namespace steamrot::paths {

/////////////////////////////////////////////////
/// @brief Provides the source directory path configured at CMake time.
///
/// This is the root directory of the project.
/////////////////////////////////////////////////
std::filesystem::path GetSourceDirectory();

/////////////////////////////////////////////////
/// @brief Provides top level data directory path.
///
/// Returns different paths based on compile-time environment:
/// - Test (default): ${SOURCE_DIR}/tests/data
/// - Debug/Prod: ${SOURCE_DIR}/data
///
/// @return std::filesystem::path The base data directory path
/////////////////////////////////////////////////
inline std::filesystem::path GetDataDirectory() {

#if defined(STEAMROT_ENV_PROD) && defined(STEAMROT_ENV_DEBUG)
#error "Cannot define both STEAMROT_ENV_PROD and STEAMROT_ENV_DEBUG"
#elif defined(STEAMROT_ENV_PROD) || defined(STEAMROT_ENV_DEBUG)
  return GetSourceDirectory() / "data";
#else
  // Default: Test environment
  return GetSourceDirectory() / "tests" / "data";
#endif
}

/////////////////////////////////////////////////
/// @brief Provides the path to the defaults directory.
///
/// Contains read-only default configuration data.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetDefaultsDirectory() {
  return GetDataDirectory() / "defaults";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the user directory.
///
/// Contains user-specific data including saves and preferences.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetUserDirectory() {
  return GetDataDirectory() / "user";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the fragments directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetFragmentDirectory() {
  return GetDataDirectory() / "fragments";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the default scenes directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetDefaultScenesDirectory() {
  return GetDefaultsDirectory() / "scenes";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the scenes directory (defaults).
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetSceneDirectory() {
  return GetDefaultScenesDirectory();
}

/////////////////////////////////////////////////
/// @brief Provides the path to the assets directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetAssetsDirectory() {
  return GetDataDirectory() / "assets";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the fonts directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetFontsDirectory() {
  return GetAssetsDirectory() / "fonts";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the ui_styles directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetUIStylesDirectory() {
  return GetDefaultsDirectory() / "ui_styles";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the default preferences directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetDefaultPreferencesDirectory() {
  return GetDefaultsDirectory() / "preferences";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the user preferences directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetUserPreferencesDirectory() {
  return GetUserDirectory() / "preferences";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the preferences directory.
///
/// Returns the default preferences directory for backwards compatibility.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetPreferencesDirectory() {
  return GetDefaultPreferencesDirectory();
}

/////////////////////////////////////////////////
/// @brief Provides the path to the user saves directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetSavesDirectory() {
  return GetUserDirectory() / "saves";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the default engine directory.
///
/// Contains default engine configuration data.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetDefaultEngineDirectory() {
  return GetDefaultsDirectory() / "engine";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the default context directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetDefaultContextDirectory() {
  return GetDefaultsDirectory() / "context";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the default scene manager directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetDefaultSceneManagerDirectory() {
  return GetDefaultsDirectory() / "scene_manager";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the default asset manager directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetDefaultAssetManagerDirectory() {
  return GetDefaultsDirectory() / "asset_manager";
}

} // namespace steamrot::paths
