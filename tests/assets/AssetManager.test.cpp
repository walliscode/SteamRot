/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for AssetManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "PathProvider.h"
#include "scene_types_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetManager is constructed correctly", "[AssetManager]") {

  steamrot::AssetManager asset_manager{steamrot::EnvironmentType::Test};
  REQUIRE_NOTHROW(asset_manager);
}
TEST_CASE("AssetManager loads scene assets correctly", "[AssetManager]") {

  steamrot::AssetManager asset_manager{steamrot::EnvironmentType::Test};

  auto result = asset_manager.LoadSceneAssets(steamrot::SceneType_TEST);

  if (!result.has_value())
    FAIL(result.error().message);

  // check if the assets were loaded correctly
  std::vector<std::string> expected_fonts = {"DaddyTimeMonoNerdFont-Regular"};
  for (const auto &font_name : expected_fonts) {
    auto font_result = asset_manager.GetFont(font_name);
    if (!font_result.has_value()) {
      FAIL(font_result.error().message);
    }
  }
}
