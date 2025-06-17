#include "AssetManager.h"
#include "SceneType.h"
#include <SFML/Graphics/Font.hpp>
#include <string>
#include <utility>

namespace steamrot {

/////////////////////////////////////////////////
AssetManager::AssetManager() : m_data_manager() {}

/////////////////////////////////////////////////
void AssetManager::LoadSceneAssets(const SceneType &scene_type) {

  // Load Scene Fonts
  AddFonts(m_data_manager.ProvideFonts(scene_type));
}

/////////////////////////////////////////////////
void AssetManager::AddFonts(
    const std::vector<std::pair<std::string, sf::Font>> &fonts) {
  // iterate through the vector of fonts and add them to the map
  for (const auto &font_pair : fonts) {
    m_fonts.insert(font_pair);
  }
};

/////////////////////////////////////////////////
const sf::Font &AssetManager::GetFont(const std::string &name) const {
  // search the map for the font and if it exists return it
  auto it = m_fonts.find(name);
  if (it != m_fonts.end()) {

    return it->second;
  } else {

    return m_fonts.begin()->second;
  }
}

} // namespace steamrot
