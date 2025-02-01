#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class AssetManager {
private:
  // maps containing the assets
  std::map<std::string, sf::Font> m_fonts;

  // ###### Fonts ######
  void LoadFonts(const std::string &scene_type);
  void AddFont(const std::string &tag, const std::string &file_name);

public:
  // will be created in the SceneManager class. It will load assets as needed
  AssetManager();

  void LoadSceneAssets(
      const std::string
          &scene_type); // load all assets for this particular scene type

  // ###### Fonts ######
  const sf::Font &GetFont(const std::string &name) const; // get a font

  // export data to json for testing
  json ToJSON();
};
