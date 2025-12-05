/////////////////////////////////////////////////
/// @file
/// @brief Implementations for asset helper functions used in tests.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "asset_test_helpers.h"
#include "catch2/catch_test_macros.hpp"
#include "provider_factory.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
void CheckAssetConfiguration(const SceneType &scene_type,
                             const AssetManager &asset_manager) {

  // get asset data using provider interface
  IAssetDataProvider &provider = GetAssetDataProvider();
  auto asset_data_result = provider.LoadSceneAssetData(scene_type);
  if (!asset_data_result.has_value()) {
    FAIL(asset_data_result.error().message);
  }
  const AssetData &asset_data = asset_data_result.value();

  // check fonts
  CheckFontConfiguration(asset_data, asset_manager);
}

/////////////////////////////////////////////////
void CheckFontConfiguration(const AssetData &asset_data,
                            const AssetManager &asset_manager) {

  // cycle through the fonts required
  for (const auto &font_data : asset_data.fonts) {

    const std::string &font_name = font_data.name;

    // check if the font is in the AssetManager
    auto font_result = asset_manager.GetFont(font_name);

    if (!font_result.has_value()) {
      FAIL(font_result.error().message);
    }
  }
}

} // namespace steamrot::tests
