/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersAssetDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersAssetDataProvider.h"
#include "assets_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<AssetData, FailInfo>
FlatbuffersAssetDataProvider::LoadAssetData() const {
  // Use existing loader
  auto fb_result = m_loader.ProvideAssetData();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const auto *fb_data = fb_result.value();

  // Convert FlatBuffers type to native struct
  AssetData native_data;

  // Convert fonts
  if (fb_data->fonts()) {
    for (const auto *font : *fb_data->fonts()) {
      FontData font_data;
      if (font->name()) {
        font_data.name = font->name()->str();
      }
      native_data.fonts.push_back(font_data);
    }
  }

  // Convert UI styles
  if (fb_data->ui_styles()) {
    for (const auto *style : *fb_data->ui_styles()) {
      if (style) {
        native_data.ui_styles.push_back(style->str());
      }
    }
  }

  return native_data;
}

/////////////////////////////////////////////////
std::expected<AssetData, FailInfo>
FlatbuffersAssetDataProvider::LoadSceneAssetData(SceneType scene_type) const {
  // Use existing loader
  auto fb_result = m_loader.ProvideAssetData(scene_type);
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const auto *fb_data = fb_result.value();

  // Convert FlatBuffers type to native struct
  AssetData native_data;

  // Convert fonts
  if (fb_data->fonts()) {
    for (const auto *font : *fb_data->fonts()) {
      FontData font_data;
      if (font->name()) {
        font_data.name = font->name()->str();
      }
      native_data.fonts.push_back(font_data);
    }
  }

  // Convert UI styles
  if (fb_data->ui_styles()) {
    for (const auto *style : *fb_data->ui_styles()) {
      if (style) {
        native_data.ui_styles.push_back(style->str());
      }
    }
  }

  return native_data;
}

} // namespace steamrot
