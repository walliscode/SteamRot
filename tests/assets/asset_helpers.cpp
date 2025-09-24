/////////////////////////////////////////////////
/// @file
/// @brief Implementations for asset helper functions used in tests.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "asset_helpers.h"
#include "FlatbuffersDataLoader.h"
#include "PathProvider.h"
#include "catch2/catch_test_macros.hpp"

namespace steamrot::tests {

/////////////////////////////////////////////////
void check_asset_configuration(const SceneType &scene_type,
                               const AssetManager &asset_manager) {

  // intialise the pathprovider
  PathProvider path_provider{EnvironmentType::Test};
  // get flatbuffer asset collection
  FlatbuffersDataLoader loader;
  auto asset_get_result = loader.ProvideAssetData(scene_type);
  if (!asset_get_result.has_value()) {
    FAIL(asset_get_result.error().message);
  }
  const AssetCollection *asset_collection = asset_get_result.value();
  REQUIRE(asset_collection != nullptr);

  // check fonts
  check_font_configuration(*asset_collection, asset_manager);
}

/////////////////////////////////////////////////
void check_font_configuration(const AssetCollection &asset_collection,
                              const AssetManager &asset_manager) {

  if (!asset_collection.fonts()) {
    // no fonts to check
    return;
  }
  // cycle through the fonts required by the Scene
  for (const auto &font : *asset_collection.fonts()) {

    const std::string font_name = font->name()->str();

    // check if the font is in the AssetManager
    auto font_result = asset_manager.GetFont(font_name);

    if (!font_result.has_value()) {
      FAIL(font_result.error().message);
    }
  }
}

} // namespace steamrot::tests
