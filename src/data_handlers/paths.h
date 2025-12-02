/////////////////////////////////////////////////
/// @file
/// @brief Namespace-based path provider using compile-time environment selection.
///
/// Environment Selection (compile-time macros):
/// - Default: Test environment (tests/data)
/// - STEAMROT_ENV_DEBUG: Debug environment (data) - same as production
/// - STEAMROT_ENV_PROD: Production environment (data)
///
/// Usage:
///   #define STEAMROT_ENV_PROD  // Before including paths.h for production
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
  static_assert(false, "Cannot define both STEAMROT_ENV_PROD and STEAMROT_ENV_DEBUG");
#elif defined(STEAMROT_ENV_PROD) || defined(STEAMROT_ENV_DEBUG)
  return GetSourceDirectory() / "data";
#else
  // Default: Test environment
  return GetSourceDirectory() / "tests" / "data";
#endif
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
/// @brief Provides the path to the scenes directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetSceneDirectory() {
  return GetDataDirectory() / "scenes";
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
  return GetDataDirectory() / "ui_styles";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the preferences directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetPreferencesDirectory() {
  return GetDataDirectory() / "preferences";
}

/////////////////////////////////////////////////
/// @brief Provides the path to the saves directory.
///
/// @return std::filesystem::path
/////////////////////////////////////////////////
inline std::filesystem::path GetSavesDirectory() {
  return GetDataDirectory() / "saves";
}

} // namespace steamrot::paths
