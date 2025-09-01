/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the AssetManager class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "FailInfo.h"
#include "FlatbuffersDataLoader.h"
#include "PathProvider.h"
#include "StylesConfigurator.h"
#include "assets_generated.h"
#include <SFML/Graphics/Font.hpp>
#include <expected>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
AssetManager::AssetManager(const EnvironmentType &env_type)
    : m_path_provider(env_type) {

      };

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> AssetManager::LoadDefaultAssets() {

  // provide Asset configuration data (not the Assets themselves)
  FlatbuffersDataLoader fb_data_loader;

  auto asset_config_result = fb_data_loader.ProvideAssetData();
  if (!asset_config_result.has_value())
    return std::unexpected<FailInfo>(asset_config_result.error());

  std::cout << "Asset configuration data loaded" << std::endl;
  const AssetCollection *asset_config = asset_config_result.value();

  ////// Load Fonts //////
  // check if there are any fonts to load
  if (asset_config->fonts()) {

    // Load Scene Fonts
    for (auto const &font_data : *asset_config->fonts()) {

      // attempt to add font
      auto add_font_result = AddFont(font_data->name()->str());

      if (!add_font_result.has_value())
        return std::unexpected<FailInfo>(add_font_result.error());
    }
  }

  ////// Load UI Styles //////
  // create a vector of style names to load and pass to LoadUIStyles
  std::vector<std::string> style_names;
  if (!asset_config->ui_styles()->empty()) {
    for (auto const &style_name : *asset_config->ui_styles()) {
      style_names.push_back(style_name->str());
    }
  }

  auto load_ui_style_resylt = LoadUIStyles(style_names);
  if (!load_ui_style_resylt.has_value())
    return std::unexpected<FailInfo>(load_ui_style_resylt.error());

  return std::monostate();
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::LoadSceneAssets(const SceneType &scene_type) {

  // provide Asset configuration data (not the Assets themselves)
  FlatbuffersDataLoader fb_data_loader;

  auto asset_config_result = fb_data_loader.ProvideAssetData(scene_type);
  if (!asset_config_result.has_value())
    return std::unexpected<FailInfo>(asset_config_result.error());
  std::cout << "Asset configuration data loaded" << std::endl;
  const AssetCollection *asset_config = asset_config_result.value();

  // return early if no data is in the AssetCollection
  if (!asset_config)
    return std::monostate();

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::AddFont(const std::string &font_name) {
  // check get font function
  auto font_dir_result = m_path_provider.GetFontsDirectory();
  if (!font_dir_result.has_value()) {
    return std::unexpected<FailInfo>(font_dir_result.error());
  }

  // generate full font file name, we are baking in .tff files here. can be
  // changed
  std::string font_file_name = font_name + ".ttf";
  std::filesystem::path font_path =
      m_path_provider.GetFontsDirectory().value() / font_file_name;

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

  // insert the font into the map
  m_fonts.insert({font_name, std::make_shared<sf::Font>(font)});

  return std::monostate{};
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::LoadUIStyles(std::vector<std::string> &style_names) {
  std::cout << "Loading UI Styles\n";
  std::cout << "Style names to load:\n";
  for (const auto &name : style_names) {
    std::cout << " - " << name << "\n";
  }
  // create StylesConfigurator object
  StylesConfigurator styles_configurator;
  // provide map of UIStyles
  auto ui_styles_map_result =
      styles_configurator.ProvideUIStylesMap(*this, style_names);
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
