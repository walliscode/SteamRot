/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to configure asset data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_asset_config.h"
#include "FailInfo.h"

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureAssetConfig(AssetConfig &asset_config,
                     const AssetConfigFbs *asset_config_fb_data) {

  // nullptr check
  if (!asset_config_fb_data) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "AssetConfigFbs data is null."});
  }

  if (asset_config_fb_data->fonts()) {
    // Configure fonts
    for (const auto *font_fb_data : *asset_config_fb_data->fonts()) {
      FontData font_data;
      font_data.name = font_fb_data->name()->str();
      asset_config.fonts.push_back(font_data);
    }
  }

  // Configure UI styles
  if (asset_config_fb_data->ui_styles()) {
    for (const auto *style_fb_data : *asset_config_fb_data->ui_styles()) {
      asset_config.ui_styles.push_back(style_fb_data->str());
    }
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
