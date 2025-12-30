/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for paths namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "paths.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <string>

TEST_CASE("GetDataDirectory returns tests/data for test builds",
          "[unit][paths]") {
  // In test builds, paths should point to tests/data
  // This is because tests do not link to environment_config
  std::filesystem::path data_dir = steamrot::paths::GetDataDirectory();
  std::string path_str = data_dir.string();
  
  // Check that the path contains "tests/data" or "tests\\data" (Windows)
  bool contains_tests_data = (path_str.find("tests/data") != std::string::npos) ||
                             (path_str.find("tests\\data") != std::string::npos);
  
  REQUIRE(contains_tests_data);
}

TEST_CASE("GetAssetsDirectory is within data directory",
          "[unit][paths]") {
  std::filesystem::path data_dir = steamrot::paths::GetDataDirectory();
  std::filesystem::path assets_dir = steamrot::paths::GetAssetsDirectory();
  
  // Assets directory should be a subdirectory of data directory
  std::string data_str = data_dir.string();
  std::string assets_str = assets_dir.string();
  
  REQUIRE(assets_str.find(data_str) == 0);
}

TEST_CASE("GetFontsDirectory is within assets directory",
          "[unit][paths]") {
  std::filesystem::path assets_dir = steamrot::paths::GetAssetsDirectory();
  std::filesystem::path fonts_dir = steamrot::paths::GetFontsDirectory();
  
  // Fonts directory should be a subdirectory of assets directory
  std::string assets_str = assets_dir.string();
  std::string fonts_str = fonts_dir.string();
  
  REQUIRE(fonts_str.find(assets_str) == 0);
}

TEST_CASE("GetDefaultsDirectory is within data directory",
          "[unit][paths]") {
  std::filesystem::path data_dir = steamrot::paths::GetDataDirectory();
  std::filesystem::path defaults_dir = steamrot::paths::GetDefaultsDirectory();
  
  // Defaults directory should be a subdirectory of data directory
  std::string data_str = data_dir.string();
  std::string defaults_str = defaults_dir.string();
  
  REQUIRE(defaults_str.find(data_str) == 0);
}

TEST_CASE("Path functions return valid filesystem paths",
          "[unit][paths]") {
  // All path functions should return valid paths
  REQUIRE_NOTHROW(steamrot::paths::GetSourceDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetDataDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetDefaultsDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetUserDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetFragmentDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetDefaultScenesDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetSceneDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetAssetsDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetFontsDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetUIStylesDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetDefaultPreferencesDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetUserPreferencesDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetPreferencesDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetSavesDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetDefaultEngineDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetDefaultContextDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetDefaultSceneManagerDirectory());
  REQUIRE_NOTHROW(steamrot::paths::GetDefaultAssetManagerDirectory());
}
