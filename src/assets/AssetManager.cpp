/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the AssetManager class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "FailInfo.h"
#include "StylesConfigurator.h"
#include "paths.h"
#include "provider_factory.h"
#include <SFML/Graphics/Font.hpp>
#include <expected>
#include <filesystem>
#include <format>
#include <memory>
#include <string>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> AssetManager::LoadDefaultAssets() {

  // Load Asset configuration data via provider interface
  IAssetDataProvider &asset_provider = GetAssetDataProvider();

  auto asset_data_result = asset_provider.LoadAssetData();
  if (!asset_data_result.has_value())
    return std::unexpected<FailInfo>(asset_data_result.error());

  const AssetData &asset_data = asset_data_result.value();

  ////// Load Fonts //////
  // Load fonts if any exist
  for (const auto &font_data : asset_data.fonts) {
    // attempt to add font
    auto add_font_result = AddFont(font_data.name);

    if (!add_font_result.has_value())
      return std::unexpected<FailInfo>(add_font_result.error());
  }

  ////// Load UI Styles //////
  if (asset_data.ui_styles.empty())
    return std::unexpected<FailInfo>(
        {FailMode::FlatbuffersDataNotFound,
         "No UI styles defined in AssetData"});

  auto load_ui_style_result = LoadUIStyles(asset_data.ui_styles);
  if (!load_ui_style_result.has_value())
    return std::unexpected<FailInfo>(load_ui_style_result.error());

  return std::monostate();
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::LoadSceneAssets(const SceneType &scene_type) {

  // Load Asset configuration data via provider interface
  IAssetDataProvider &asset_provider = GetAssetDataProvider();

  auto asset_data_result = asset_provider.LoadSceneAssetData(scene_type);
  if (!asset_data_result.has_value())
    return std::unexpected<FailInfo>(asset_data_result.error());

  // Scene assets are loaded on demand, return success
  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::AddFont(const std::string &font_name) {

  // get font directory
  std::filesystem::path font_dir = paths::GetFontsDirectory();

  // generate full font file name, we are baking in .tff files here. can be
  // changed
  std::string font_file_name = font_name + ".ttf";
  std::filesystem::path font_path = font_dir / font_file_name;

  // check if files exists
  if (!std::filesystem::exists(font_path)) {
    // construct error message
    std::string error_message =
        std::format("Font file not found: {}", font_path.string());
    // create FailInfo object
    FailInfo fail_info{FailMode::FileNotFound, error_message};

    // return early with FileNotFound error
    return std::unexpected<FailInfo>(fail_info);
  }
  // create a new font object
  sf::Font font;

  // load the font from file
  bool sucess = font.openFromFile(font_path);
  if (!sucess) {

    // construct error message
    std::string error_message =
        std::format("Failed to load font from file: {}", font_path.string());
    // create FailInfo object
    FailInfo fail_info{FailMode::FileNotFound, error_message};
    // return early with FileNotFound error
    return std::unexpected<FailInfo>(fail_info);
  }
  // unsmooth the font
  font.setSmooth(false);

  // insert the font into the map
  m_fonts.insert({font_name, std::make_shared<sf::Font>(font)});

  return std::monostate{};
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::LoadUIStyles(std::vector<std::string> &style_names) {

  // create StylesConfigurator object
  StylesConfigurator styles_configurator;
  // provide map of UIStyles
  auto ui_styles_map_result = styles_configurator.ProvideUIStylesMap(
      *this, style_names);

  if (!ui_styles_map_result.has_value()) {
    return std::unexpected<FailInfo>(ui_styles_map_result.error());
  }
  m_ui_styles = ui_styles_map_result.value();

  return std::monostate();
}
/////////////////////////////////////////////////
std::expected<std::shared_ptr<const sf::Font>, FailInfo>
AssetManager::GetFont(const std::string &font_name) const {

  // check if font exists in the map
  auto it = m_fonts.find(font_name);

  if (it != m_fonts.end()) {
    // have to use cref to create a reference wrapper
    return it->second;
  }
  // construct error message
  std::string error_message =
      std::format("Font not found in font map: {}", font_name);
  FailInfo fail_info{FailMode::FileNotFound, error_message};
  // return early with FileNotFound error
  return std::unexpected<FailInfo>(fail_info);
}

/////////////////////////////////////////////////
const std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &
AssetManager::GetAllFonts() const {
  return m_fonts;
}

/////////////////////////////////////////////////
const UIStyle &AssetManager::GetDefaultUIStyle() const {
  auto it = m_ui_styles.find("default");
  if (it != m_ui_styles.end()) {
    return it->second;
  } else {
    throw std::runtime_error("Default UIStyle not found");
  }
}
/////////////////////////////////////////////////
const std::unordered_map<std::string, UIStyle> &
AssetManager::GetAllUIStyles() const {
  return m_ui_styles;
}
} // namespace steamrot
