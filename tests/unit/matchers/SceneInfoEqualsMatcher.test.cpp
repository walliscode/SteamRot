/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneInfoEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneInfoEqualsMatcher.h"
#include "SceneInfo.h"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("SceneInfoEqualsMatcher works correctly",
          "[unit][SceneInfo][matcher]") {
  // Define expected SceneInfo
  steamrot::SceneInfo expected;
  expected.type = steamrot::SceneType::TEST;
  uuids::uuid id = uuids::uuid_system_generator{}();
  expected.id = id;

  // intialize actual SceneInfo
  steamrot::SceneInfo actual;

  SECTION("Matcher detects differences in scene type") {

    actual.type = steamrot::SceneType::TITLE;
    actual.id = id;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSceneInfo(expected));
  }
  SECTION("Matcher detects differences in scene ID") {

    actual.type = steamrot::SceneType::TEST;
    actual.id = uuids::uuid_system_generator{}();
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSceneInfo(expected));
  }
  SECTION("Matcher detects equality") {
    actual.type = steamrot::SceneType::TEST;
    actual.id = id;
    REQUIRE_THAT(actual, steamrot::tests::EqualsSceneInfo(expected));
  }
}
