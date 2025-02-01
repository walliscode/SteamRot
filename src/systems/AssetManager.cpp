#include "AssetManager.h"
#include "general_util.h"
#include <iostream>

AssetManager::AssetManager() : m_fonts() {}

void AssetManager::LoadSceneAssets(const std::string &scene_type) {
  // load the fonts for the scene
  LoadFonts(scene_type);
}

void AssetManager::LoadFonts(const std::string &scene_type) {

  // create the file name. check naming convention carefully
  const std::string file_name =
      std::string(RESOURCE_DIR) + "/jsons/" + scene_type + "_fonts.json";

  if (!utils::fileExists(file_name)) {
    std::cout << "File does not exist: " << file_name << std::endl;
    return;
  }
  std::cout << "######  Loading fonts ###### " << std::endl;

  // load the json file
  std::ifstream f(file_name);
  json font_data = json::parse(f);

  // iterate through the json object and add the fonts to the asset manager
  for (auto &font : font_data) {
    std::string font_tag = font["tag"];
    std::string font_file = font["file"];

    // check to see if font already exists
    auto it = m_fonts.find(font_tag);
    if (it != m_fonts.end()) {
      std::cout << "Font already loaded: " << font_tag << std::endl;
      continue; // skip this font
    }
    AddFont(font_tag, font_file);
  };
};

void AssetManager::AddFont(const std::string &tag,
                           const std::string &file_name) {
  // provide path to font file
  std::string path = std::string(RESOURCE_DIR) + "/fonts/" + file_name;

  // create font object with path to constructor
  std::cout << "Loading font: " << path << std::endl;
  sf::Font font{file_name};

  // add font to map
  m_fonts.insert({tag, font});

  std::cout << "Loaded font: " << tag << std::endl;
}

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
