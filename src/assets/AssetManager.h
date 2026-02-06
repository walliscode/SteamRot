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
#include "DataAccessFactory.h"
#include "FailInfo.h"
#include "GrimoireMachina.h"
#include "IFontProvider.h"
#include "UIStyle.h"
#include <SFML/Graphics.hpp>
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace steamrot {

class AssetManager : public IFontProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Reference to the DataAccessFactory for loading assets.
  /////////////////////////////////////////////////
  DataAccessFactory &m_data_access_factory;

  /////////////////////////////////////////////////
  /// @brief The GrimoireMachina instance for the Game
  /////////////////////////////////////////////////
  std::unique_ptr<GrimoireMachina> m_grimoire_machina;

  /////////////////////////////////////////////////
  /// @brief Member variable contining all the fonts for the game.
  /////////////////////////////////////////////////
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_fonts;

  /////////////////////////////////////////////////
  /// @brief Member variable containing all the UI styles for the game.
  /////////////////////////////////////////////////
  std::unordered_map<std::string, UIStyle> m_ui_styles;

  /////////////////////////////////////////////////
  /// @brief Adds a single font to the Font map in the AssetManager
  ///
  /// @param font_name The name of the font as a string
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> AddFont(const std::string &font_name);

  /////////////////////////////////////////////////
  /// @brief Wrapper function to set up the GrimoireMachina instance using the
  /// provider
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> SetUpGrimoireMachina();

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for AssetManager taking a reference to a
  /// DataAccessFactory
  ///
  /// @param data_access_factory DataAccessFactory reference for loading assets
  /////////////////////////////////////////////////
  AssetManager(DataAccessFactory &data_access_factory);

  /////////////////////////////////////////////////
  /// @brief Contaienr function to load any default assets.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> Startup();

  /////////////////////////////////////////////////
  /// @brief Load assets from the provided AssetConfig data
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  LoadAssetsFromConfig(const AssetConfig asset_config);

  /////////////////////////////////////////////////
  /// @brief Load all UI styles
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> LoadUIStyles();

  /////////////////////////////////////////////////
  /// @brief return reference to the GrimoireMachina5instance
  /////////////////////////////////////////////////
  std::expected<GrimoireMachina *, FailInfo> GetGrimoireMachina();

  /////////////////////////////////////////////////
  /// @brief Implements IFontProvider::GetFont
  ///
  /// @param font_name String name of the font to retrieve
  /////////////////////////////////////////////////
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
  GetFont(const std::string &font_name) const override;

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
