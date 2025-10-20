/////////////////////////////////////////////////
/// @file
/// @brief Unit test for PathProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PathProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PathProvider must be initiated with a valid environment",
          "[PathProvider]") {

  REQUIRE_THROWS(steamrot::PathProvider{steamrot::EnvironmentType::None});
}
TEST_CASE("PathProvider enviroment can only be initiated once",
          "[PathProvider]") {

  // create PathProvider instance with production environment
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Production};

  // try to create another PathProvider instance with test environment, this
  // should throw an exception
  REQUIRE_THROWS(steamrot::PathProvider{steamrot::EnvironmentType::Test});
}

TEST_CASE("PathProvider can instantiated with a None enviroment if already set",
          "[PathProvider]") {
  // create PathProvider instance with production environment
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Production};
  // try to create another PathProvider instance with None environment, this
  // should not throw an exception
  REQUIRE_NOTHROW(steamrot::PathProvider{});
}
TEST_CASE("PathProvider intiated with a test environment", "[PathProvider]") {

  // create PathProvider instance with test environment
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};

  REQUIRE(path_provider.GetDataDirectory() == "/home/runner/work/SteamRot/SteamRot/tests/data");

  REQUIRE(path_provider.GetFragmentDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/tests/data/fragments");

  REQUIRE(path_provider.GetSceneDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/tests/data/scenes");

  REQUIRE(path_provider.GetUIStylesDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/tests/data/ui_styles");

  REQUIRE(path_provider.GetAssetsDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/tests/data/assets");

  REQUIRE(path_provider.GetFontsDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/tests/data/assets/fonts");
}

TEST_CASE("PathProvider intiated with a production environment",
          "[PathProvider]") {
  // create PathProvider instance with production environment
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Production};

  REQUIRE(path_provider.GetDataDirectory() == "/home/runner/work/SteamRot/SteamRot/data");

  REQUIRE(path_provider.GetFragmentDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/data/fragments");

  REQUIRE(path_provider.GetSceneDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/data/scenes");

  REQUIRE(path_provider.GetUIStylesDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/data/ui_styles");

  REQUIRE(path_provider.GetAssetsDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/data/assets");

  REQUIRE(path_provider.GetFontsDirectory() ==
          "/home/runner/work/SteamRot/SteamRot/data/assets/fonts");
}
