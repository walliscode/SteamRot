/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the AssetManager class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"

#include "PathProvider.h"
#include "UIStyle.h"
#include "scene_types_generated.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace steamrot {

class AssetManager {
private:
  /////////////////////////////////////////////////
  /// @brief Member variable contining all the fonts for the game.
  /////////////////////////////////////////////////
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_fonts;

  /////////////////////////////////////////////////
  /// @brief Member variable containing all the UI styles for the game.
  /////////////////////////////////////////////////
  std::unordered_map<std::string, UIStyle> m_ui_styles;

  /////////////////////////////////////////////////
  /// @brief PathProvider for getting asset paths
  /////////////////////////////////////////////////
  PathProvider m_path_provider;

  /////////////////////////////////////////////////
  /// @brief Adds a single font to the Font map in the AssetManager
  ///
  /// @param font_name The name of the font as a string
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> AddFont(const std::string &font_name);

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking an EnvironmentType parameter for setting up
  /// PathProvider.
  ///
  /// @param env_type Enum representing the environment type (e.g., development,
  /// production).
  /////////////////////////////////////////////////
  AssetManager(const EnvironmentType &env_type = EnvironmentType::None);

  /////////////////////////////////////////////////
  /// @brief Load all the default assets for the game, independent of scene
  /// type.
  ///
  /// This serves as a wrapper function to load all the default assets
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> LoadDefaultAssets();

  /////////////////////////////////////////////////
  /// @brief Load all the assets for a given scene type.
  ///
  /// @param scene_type Enum representing the derived scene type to load assets
  /// for.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  LoadSceneAssets(const SceneType &scene_type);

  /////////////////////////////////////////////////
  /// @brief Load UIStyle data to UIStyle map.
  ///
  /// @param style_data Flabuffer data containging UIStyle configuration data.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  LoadUIStyles(std::vector<std::string> &style_names);

  /////////////////////////////////////////////////
  /// @brief Return a shared_ptr to a font from the AssetManager
  ///
  /// @param font_name String representing the name of the font to retrieve.
  /////////////////////////////////////////////////
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
  GetFont(const std::string &font_name) const;

  /////////////////////////////////////////////////
  /// @brief Returns a const reference to the entire font map.
  /////////////////////////////////////////////////
  const std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &
  GetAllFonts() const;

  /////////////////////////////////////////////////
  /// @brief Convenience function to get the default UIStyle
  ///
  /// @return reference to the default UIStyle
  /////////////////////////////////////////////////
  const UIStyle &GetDefaultUIStyle() const;

  const std::unordered_map<std::string, UIStyle> &GetAllUIStyles() const;
};
}; // namespace steamrot
