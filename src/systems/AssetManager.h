#pragma once
#include "DataManager.h"
#include "SceneType.h"
#include <SFML/Graphics.hpp>

#include <SFML/Graphics/Font.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

namespace steamrot {

class AssetManager {
private:
  /////////////////////////////////////////////////
  /// @brief Member variable contining all the fonts for the game.
  /////////////////////////////////////////////////
  std::unordered_map<std::string, sf::Font> m_fonts;

  /////////////////////////////////////////////////
  /// @brief Instance of the DataManager to handle loading of assets from file.
  /////////////////////////////////////////////////
  DataManager m_data_manager;

  void AddFonts(const std::vector<std::pair<std::string, sf::Font>> &fonts);

public:
  AssetManager();

  /////////////////////////////////////////////////
  /// @brief Load all the assets for a given scene type.
  ///
  /// @param scene_type Enum representing the derived scene type to load assets
  /// for.
  /////////////////////////////////////////////////
  void LoadSceneAssets(const SceneType &scene_type);

  /////////////////////////////////////////////////
  /// @brief Return a reference to a font by name.
  ///
  /// @param name String name of the font to retrieve.
  /////////////////////////////////////////////////
  const sf::Font &GetFont(const std::string &name) const; // get a font
};
}; // namespace steamrot
