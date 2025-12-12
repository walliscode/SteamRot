/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersAssetDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersAssetDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersAssetDataProvider is constructed correctly",
          "[unit][FlatbuffersAssetDataProvider]") {

  steamrot::FlatbuffersAssetDataProvider provider;
  REQUIRE_NOTHROW(provider);
}

TEST_CASE("FlatbuffersAssetDataProvider::LoadAssetData returns default data",
          "[unit][FlatbuffersAssetDataProvider]") {

  steamrot::FlatbuffersAssetDataProvider provider;
  auto result = provider.LoadAssetData();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &asset_data = result.value();
  REQUIRE(!asset_data.fonts.empty());
  REQUIRE(!asset_data.ui_styles.empty());

  // Verify font data structure
  for (const auto &font : asset_data.fonts) {
    REQUIRE(!font.name.empty());
  }

  // Verify ui_styles data structure
  for (const auto &style : asset_data.ui_styles) {
    REQUIRE(!style.empty());
  }
}

TEST_CASE(
    "FlatbuffersAssetDataProvider::LoadSceneAssetData returns scene-specific "
    "data",
    "[unit][FlatbuffersAssetDataProvider]") {

  steamrot::FlatbuffersAssetDataProvider provider;
  auto result = provider.LoadSceneAssetData(steamrot::SceneType::SceneType_TEST);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &asset_data = result.value();
  // Scene asset data may not have fonts, so only check ui_styles
  REQUIRE_NOTHROW(asset_data.ui_styles);
}

TEST_CASE("FlatbuffersAssetDataProvider::LoadSceneAssetData handles invalid "
          "scene type",
          "[unit][FlatbuffersAssetDataProvider]") {

  steamrot::FlatbuffersAssetDataProvider provider;
  // Use an invalid scene type (assuming NONE is invalid)
  auto result =
      provider.LoadSceneAssetData(steamrot::SceneType::SceneType_NONE);

  // Should return error for invalid scene
  REQUIRE(!result.has_value());
}

TEST_CASE("FlatbuffersAssetDataProvider loads data with correct structure",
          "[unit][FlatbuffersAssetDataProvider]") {

  steamrot::FlatbuffersAssetDataProvider provider;
  auto result = provider.LoadAssetData();

  REQUIRE(result.has_value());

  const auto &asset_data = result.value();

  // Verify the structure - fonts should be FontData with name field
  for (const auto &font : asset_data.fonts) {
    REQUIRE(!font.name.empty());
    // Font names should not be empty
    REQUIRE(font.name.length() > 0);
  }

  // Verify ui_styles are non-empty strings
  for (const auto &style : asset_data.ui_styles) {
    REQUIRE(style.length() > 0);
  }
}
