#include "DataManager.h"

#include "SchemaChecker.h"
#include "log_handler.h"
#include "steamrot_directory_paths.h"

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
  }
};

////////////////////////////////////////////////////////////
json DataManager::LoadJsonData(const std::filesystem::path &file_path) {

  // check file exists, this is a go/no go checkpoint
  CheckFileExists(file_path);

  // load json data from file
  std::ifstream file(file_path);

  // return json object
  return nlohmann::json::parse(file);
}

////////////////////////////////////////////////////////////
json DataManager::LoadSceneDataFromJson(std::string scene_identifier) {

  // load schema data for scene from file
  json scene_schema = LoadJsonData(getSchemaFolder() / "scene.schema.json");

  // create SchemaChecker object
  SchemaChecker schema_checker(scene_schema);

  // load scene data from file
  json scene_data =
      LoadJsonData(getSceneFolder() / (scene_identifier + ".data.json"));

  // check scene data against schema (error checking in function)
  schema_checker.CheckJSON(scene_data);

  return scene_data;
}
////////////////////////////////////////////////////////////
json DataManager::LoadThemeData(const std::string &theme_name) {
  // load schema data for theme from file
  json theme_schema = LoadJsonData(getSchemaFolder() / "theme.schema.json");
  // create SchemaChecker object
  SchemaChecker schema_checker(theme_schema);
  // load theme data from file
  json theme_data =
      LoadJsonData(getThemesFolder() / (theme_name + ".data.json"));
  // check theme data against schema (error checking in function)
  schema_checker.CheckJSON(theme_data);
  return theme_data;
}
} // namespace steamrot
