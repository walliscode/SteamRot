#include "DataManager.h"

#include "log_handler.h"
#include "spdlog/common.h"
#include "steamrot_directory_paths.h"
#include "themes_generated.h"
#include <fstream>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

using json = nlohmann::json;

namespace steamrot {

////////////////////////////////////////////////////////////
void DataManager::CheckFileExists(const std::filesystem::path &file_path) {

  if (!std::filesystem::exists(file_path)) {
    // generate error message
    std::string message{"Target file does not exist: " + file_path.string()};

    // pass message through the log handler
    steamrot::log_handler::ProcessLog(
        spdlog::level::err, steamrot::log_handler::LogCode::kFileNotFound,
        message);
  } else {
    // file exists, log info message
    steamrot::log_handler::ProcessLog(spdlog::level::info,
                                      steamrot::log_handler::LogCode::kNoCode,
                                      "File exists: " + file_path.string());
  }
};

////////////////////////////////////////////////////////////
char *DataManager::LoadBinaryData(const std::filesystem::path &file_path) {
  // check file exists, this is a go/no go checkpoint
  CheckFileExists(file_path);

  // open file in binary mode
  std::ifstream infile;
  infile.open(file_path, std::ios::binary | std::ios::in);
  infile.seekg(0, std::ios::end);
  int length = infile.tellg();
  infile.seekg(0, std::ios::beg);
  char *data = new char[length];
  infile.read(data, length);
  infile.close();
  // log info message
  steamrot::log_handler::ProcessLog(
      spdlog::level::info, steamrot::log_handler::LogCode::kNoCode,
      "Loaded binary data from file: " + file_path.string());
  return data;
}

////////////////////////////////////////////////////////////
const themes::UIObjects *
DataManager::ProvideThemeData(const std::string &theme_name) {
  // load theme data from binary into buffer
  char *theme_data =
      LoadBinaryData(getThemesFolder() / (theme_name + ".themes.bin"));

  // return flatbuffers data from binary buffer
  return themes::GetUIObjects(theme_data);
}

////////////////////////////////////////////////////////////
const SceneData *DataManager::ProvideSceneData(const SceneType &scene_type) {
  // get string identifier for scene type
  std::string scene_identifier;

  switch (scene_type) {
  case SceneType::SceneType_TITLE: {
    scene_identifier = "title";
    break;
  }
  case SceneType::SceneType_CRAFTING: {
    scene_identifier = "crafting";
    break;
  }
  }
  // load scene data from binary into buffer
  char *scene_data =
      LoadBinaryData(getScenesFolder() / (scene_identifier + ".scenes.bin"));

  std::cout << "Loading scene data for: " << scene_identifier << std::endl;
  // return flatbuffers data from binary buffer
  return GetSceneData(scene_data);
}

/////////////////////////////////////////////////
std::vector<std::pair<std::string, sf::Font>>
DataManager::ProvideFonts(const SceneType &scene_type) {

  // create vector to hold fonts
  std::vector<std::pair<std::string, sf::Font>> fonts;

  // get SceneData for the given scene type
  const SceneData *scene_data = ProvideSceneData(scene_type);

  // check if fonts are defined in the scene data
  if (scene_data->assets()->fonts()->empty()) {
    return fonts;
  }

  // iterate over each font in the scene data
  for (const auto *font : *scene_data->assets()->fonts()) {

    // create a new font object
    sf::Font new_font;

    // full font name
    std::string font_file_name = font->name()->str() + ".ttf";
    // load the font from file
    if (new_font.openFromFile(getFontsFolder() / font_file_name)) {
      // add the font to the vector with its tag
      fonts.emplace_back(font->name()->str(), new_font);

    } else {
      // log error if font fails to load
      steamrot::log_handler::ProcessLog(
          spdlog::level::err, steamrot::log_handler::LogCode::kFileNotFound,
          "Failed to load font: " + font->name()->str());
    }
  }

  return fonts;
}

} // namespace steamrot
