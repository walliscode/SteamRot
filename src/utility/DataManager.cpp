#include "DataManager.h"

#include "SchemaChecker.h"
#include "log_handler.h"
#include "spdlog/common.h"
#include "steamrot_directory_paths.h"
#include "themes_generated.h"

#include <fstream>

using json = nlohmann::json;

namespace steamrot {
////////////////////////////////////////////////////////////
void DataManager::LoadData() {};

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
json DataManager::LoadJsonData(const std::filesystem::path &file_path) {

  // check file exists, this is a go/no go checkpoint
  CheckFileExists(file_path);
  // load json data from file
  std::ifstream file(file_path);

  // return json object
  json data = json::parse(file);
  // log info message
  steamrot::log_handler::ProcessLog(
      spdlog::level::info, steamrot::log_handler::LogCode::kNoCode,
      "Loaded JSON data from file: " + file_path.string());
  return data;
}

////////////////////////////////////////////////////////////
json DataManager::LoadSceneDataFromJson(std::string scene_identifier) {

  // load schema data for scene from file
  json scene_schema = LoadJsonData(getSchemaFolder() / "scene.schema.json");

  // create SchemaChecker object
  SchemaChecker schema_checker(scene_schema);

  // load scene data from file
  json scene_data =
      LoadJsonData(getScenesFolder() / (scene_identifier + ".data.json"));

  // check scene data against schema (error checking in function)
  schema_checker.CheckJSON(scene_data);

  return scene_data;
}
////////////////////////////////////////////////////////////
json DataManager::LoadThemeData(const std::string &theme_name) {
  // load schema data for theme from file
  json theme_schema = LoadJsonData(getSchemaFolder() / "themes.schema.json");
  // create SchemaChecker object
  SchemaChecker schema_checker(theme_schema);
  // load theme data from file
  json theme_data =
      LoadJsonData(getThemesFolder() / (theme_name + ".data.json"));
  // check theme data against schema (error checking in function)
  schema_checker.CheckJSON(theme_data);
  return theme_data;
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
const SceneData *
DataManager::ProvideSceneData(const std::string &scene_identifier) {
  // load scene data from binary into buffer
  char *scene_data =
      LoadBinaryData(getScenesFolder() / (scene_identifier + ".scene.bin"));
  // return flatbuffers data from binary buffer
  return GetSceneData(scene_data);
}

} // namespace steamrot
