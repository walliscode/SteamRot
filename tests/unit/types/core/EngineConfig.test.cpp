/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FailInfo type
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineConfig.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EngineConfig has correct default values", "[types]") {
  steamrot::EngineConfig config;
  // Test DisplayConfig defaults
  REQUIRE(config.display.window_title == "SteamRot");
  REQUIRE(config.display.framerate_limit == 60);
  REQUIRE(config.display.fullscreen == false);
  REQUIRE(config.display.vsync == true);
  // Test UserPreferencesConfig defaults
  REQUIRE(config.user_preferences.master_volume == 1.0f);
  REQUIRE(config.user_preferences.show_fps == false);
  REQUIRE(config.user_preferences.preferred_language == "en");
}
