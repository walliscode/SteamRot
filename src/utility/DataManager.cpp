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
EntityConfigData
DataManager::LoadSceneDataFromJson(std::string scene_identifier) {

  // load schema data for scene from file
  json scene_schema = LoadJsonData(getSceneFolder() / "scene.schema.json");

  // create SchemaChecker object
  SchemaChecker schema_checker(scene_schema);

  // load scene data from file
  json scene_data =
      LoadJsonData(getSceneFolder() / (scene_identifier + ".data.json"));

  // check scene data against schema (error checking in function)
  schema_checker.CheckJSON(scene_data);

  return EntityConfigData(scene_data);
}

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

} // namespace steamrot
