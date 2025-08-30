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
#include "assets_generated.h"
#include <SFML/Graphics/Font.hpp>
#include <expected>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
AssetManager::AssetManager(const EnvironmentType &env_type)
    : m_path_provider(env_type) {};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::LoadSceneAssets(const SceneType &scene_type) {

  std::cout << "[DEBUG] AssetManager::LoadSceneAssets called for scene type: "
            << static_cast<int>(scene_type) << std::endl;
  // provide Asset configuration data (not the Assets themselves)
  FlatbuffersDataLoader fb_data_loader;

  auto asset_config_result = fb_data_loader.ProvideAssetData(scene_type);
  if (!asset_config_result.has_value())
    return std::unexpected<FailInfo>(asset_config_result.error());

  const AssetCollection *asset_config = asset_config_result.value();

  std::cout << "[DEBUG] AssetManager::LoadSceneAssets: "
            << "Asset configuration loaded successfully." << std::endl;
  if (!asset_config->fonts()->empty()) {

    std::cout << "[DEBUG] AssetManager::LoadSceneAssets: "
              << "Loading fonts for scene type: "
              << static_cast<int>(scene_type) << std::endl;
    // Load Scene Fonts
    for (auto const &font_data : *asset_config->fonts()) {

      std::cout << "[DEBUG] AssetManager::LoadSceneAssets: "
                << "Loading font: " << font_data->name()->str() << std::endl;
      // attempt to add font
      auto add_font_result = AddFont(font_data->name()->str());

      if (!add_font_result.has_value())
        return std::unexpected<FailInfo>(add_font_result.error());
      std::cout << "[DEBUG] AssetManager::LoadSceneAssets: "
                << "Font loaded successfully: " << font_data->name()->str()
                << std::endl;
    }
  }
  std::cout << "[DEBUG] AssetManager::LoadSceneAssets: "
            << "Fonts loaded successfully." << std::endl;
  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::AddFont(const std::string &font_name) {
  std::cout << "[DEBUG] AssetManager::AddFont called for font: " << font_name
            << std::endl;
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
  std::cout << "[DEBUG] AssetManager::AddFont: "
            << "Font file found: " << font_path.string() << std::endl;
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

  std::cout << "[DEBUG] AssetManager::AddFont: "
            << "Font loaded successfully: " << font_name << std::endl;
  // insert the font into the map
  m_fonts.insert({font_name, std::make_shared<sf::Font>(font)});

  std::cout << "[DEBUG] AssetManager::AddFont: "
            << "Font added to map: " << font_name << std::endl;
  return std::monostate{};
}
/////////////////////////////////////////////////
std::expected<std::shared_ptr<const sf::Font>, FailInfo>
AssetManager::GetFont(const std::string &font_name) const {

  // check if font exists in the map
  auto it = m_fonts.find(font_name);

  if (it != m_fonts.end()) {
    // have to use cref to create a reference wrapper
    return std::cref(it->second);
  }
  // construct error message
  std::string error_message =
      std::format("Font not found in font map: {}", font_name);
  FailInfo fail_info{FailMode::FileNotFound, error_message};
  // return early with FileNotFound error
  return std::unexpected<FailInfo>(fail_info);
}
} // namespace steamrot
