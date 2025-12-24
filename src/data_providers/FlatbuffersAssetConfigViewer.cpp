/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersAssetConfigViewer.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersAssetConfigViewer.h"
#include "FailInfo.h"

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersAssetConfigViewer::FlatbuffersAssetConfigViewer(
    const AssetConfigFbs *asset_config_fbs)
    : m_asset_config_fbs(asset_config_fbs) {}

/////////////////////////////////////////////////
std::expected<AssetConfig, FailInfo>
FlatbuffersAssetConfigViewer::ProvideAssetConfig() const {

  // Check for null pointer
  if (m_asset_config_fbs == nullptr) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "AssetConfigFbs pointer is null"});
  }
  // Convert FlatBuffers type to native struct
  AssetConfig native_data;
  // Convert fonts
  if (m_asset_config_fbs->fonts()) {
    for (const auto *font : *m_asset_config_fbs->fonts()) {
      FontData font_data;
      if (font->name()) {
        font_data.name = font->name()->str();
      }
      native_data.fonts.push_back(font_data);
    }
  }
  // Convert UI styles
  if (m_asset_config_fbs->ui_styles()) {
    for (const auto *style : *m_asset_config_fbs->ui_styles()) {
      if (style) {
        native_data.ui_styles.push_back(style->str());
      }
    }
  }
  return native_data;
}

} // namespace steamrot
