/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FailInfo type
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneInfo.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneInfo has correct default values", "[SceneInfo]") {
  steamrot::SceneInfo scene_info;

  // uuid should be nil by default so we don't introduce accidental IDs
  REQUIRE(scene_info.id.is_nil());
  REQUIRE(scene_info.type == steamrot::SceneType::SceneType_UNKNOWN);
}
