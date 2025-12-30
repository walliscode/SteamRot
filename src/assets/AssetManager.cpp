/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the AssetManager class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "FailInfo.h"
#include "FlatbuffersUIStyleDataProvider.h"
#include "paths.h"
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
AssetManager::AssetManager(DataAccessFactory &data_access_factory)
    : m_data_access_factory(data_access_factory) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::LoadAssets(const AssetConfig asset_config) {

  ////// Load Fonts //////
  // Load fonts if any exist
  for (const auto &font_data : asset_config.fonts) {
    // attempt to add font
    auto add_font_result = AddFont(font_data.name);

    if (!add_font_result.has_value())
      return std::unexpected<FailInfo>(add_font_result.error());
  }

  std::cout << "Loaded " << m_fonts.size() << " fonts." << std::endl;
  ////// Load UI Styles //////
  if (asset_config.ui_styles.empty())
    return std::unexpected<FailInfo>({FailMode::FlatbuffersDataNotFound,
                                      "No UI styles defined in AssetData"});

  auto load_ui_style_result = LoadUIStyles();
  if (!load_ui_style_result.has_value())
    return std::unexpected<FailInfo>(load_ui_style_result.error());

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
std::expected<std::monostate, FailInfo> AssetManager::LoadUIStyles() {

  // create UIStyleDataProivder
  FlatbuffersUIStyleDataProvider ui_style_provider(m_fonts);

  std::cout << "Loading UI styles from flatbuffers..." << std::endl;

  auto ui_style_data_result = ui_style_provider.ProvideUIStyles();
  if (!ui_style_data_result.has_value())
    return std::unexpected<FailInfo>(ui_style_data_result.error());

  std::cout << "Loaded " << ui_style_data_result.value().size()
            << " UI styles from flatbuffers." << std::endl;
  // for each UIStyle object, add to map
  for (const auto &ui_style : ui_style_data_result.value()) {
    auto insert_result = m_ui_styles.insert({ui_style.name, ui_style});
    if (!insert_result.second)
      return std::unexpected<FailInfo>(
          {FailMode::NotAddedToMap,
           std::format("Failed to insert UIStyle: {}", ui_style.name)});
  }

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::shared_ptr<const sf::Font>, FailInfo>
AssetManager::GetFont(const std::string &font_name) const {

  // search for font in map
  auto it = m_fonts.find(font_name);
  if (it != m_fonts.end()) {
    return it->second;
  } else {
    return std::unexpected<FailInfo>(
        {FailMode::FileNotFound, std::format("Font not found: {}", font_name)});
  }
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
