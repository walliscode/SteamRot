/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for AssetManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "DataAccessFactory.h"
#include "asset_test_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetManager is constructed correctly", "[unit][AssetManager]") {
  steamrot::DataAccessFactory data_access_factory;
  steamrot::AssetManager asset_manager(data_access_factory);
  REQUIRE_NOTHROW(asset_manager);
}

TEST_CASE("AssetManager::LoadDefaultAssets works correctly",
          "[unit][AssetManager]") {
  steamrot::DataAccessFactory data_access_factory;
  steamrot::AssetManager asset_manager(data_access_factory);

  auto load_result = asset_manager.LoadDefaultAssets();
  if (!load_result.has_value())
    FAIL(load_result.error().message);

  // create a list of expected fonts
  std::vector<std::string> expected_fonts = {"DaddyTimeMonoNerdFont-Regular"};

  // get the fonts map and check
  const auto &fonts = asset_manager.GetAllFonts();
  for (const auto &font_name : expected_fonts) {
    REQUIRE(fonts.contains(font_name));
    REQUIRE(fonts.at(font_name) != nullptr);
  }

  // get the UIStyles map
  const auto &ui_styles = asset_manager.GetAllUIStyles();
  REQUIRE(!ui_styles.empty());
  REQUIRE(ui_styles.contains("default"));
}
TEST_CASE("AssetManager loads scene assets correctly", "[unit][AssetManager]") {

  steamrot::DataAccessFactory data_access_factory;
  steamrot::AssetManager asset_manager(data_access_factory);

  auto result = asset_manager.LoadSceneAssets(steamrot::SceneType_TEST);

  if (!result.has_value())
    FAIL(result.error().message);

  // check if the assets were loaded correctly
  steamrot::tests::CheckAssetConfiguration(steamrot::SceneType_TEST,
                                           asset_manager);
}
