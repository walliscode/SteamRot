/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for AssetManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "PathProvider.h"
#include "asset_helpers.h"
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
  steamrot::tests::check_asset_configuration(steamrot::SceneType_TEST,
                                             asset_manager);
}
