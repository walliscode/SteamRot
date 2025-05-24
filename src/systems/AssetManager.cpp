#include "AssetManager.h"

#include <iostream>

AssetManager::AssetManager() : m_fonts() {}

void AssetManager::LoadSceneAssets(const std::string &scene_type) {
  // load the fonts for the scene
  LoadFonts(scene_type);
}

void AssetManager::LoadFonts(const std::string &scene_type) {

};

void AssetManager::AddFont(const std::string &tag,
                           const std::string &file_name) {}

const sf::Font &AssetManager::GetFont(const std::string &name) const {
  // search the map for the font and if it exists return it
  auto it = m_fonts.find(name);
  if (it != m_fonts.end()) {
    std::cout << "Success: Font '" << name << "' found." << std::endl;
    return it->second;
  } else {
    std::cout << "Error: Font '" << name << "' not found." << std::endl;
    return m_fonts.begin()->second;
  }
}

json AssetManager::ToJSON() {

  // create json object
  json j;

  // return all font information
  j["fonts"] = {};
  for (auto &pair : m_fonts) {
    std::string fontTag = pair.first;
    std::string fontFamily = pair.second.getInfo().family;
    j["fonts"][fontTag] = fontFamily;
  }

  return j;
}
