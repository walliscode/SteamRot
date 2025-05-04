#include "DataManager.h"
#include "SchemaChecker.h"
#include "log_handler.h"
#include <fstream>

////////////////////////////////////////////////////////////
void DataManager::LoadData() {};

////////////////////////////////////////////////////////////
EntityConfigData DataManager::LoadSceneDataFromJson(nlohmann::json scene_data) {

  // check json is valid with schema checker
  SchemaChecker schema_checker(SchemaType::kSceneSchema);
  bool proceed = schema_checker.CheckJSON(scene_data);

  return EntityConfigData(scene_data);
}

////////////////////////////////////////////////////////////
void DataManager::CheckFileExists(const std::string &file_path) {

  if (!std::filesystem::exists(file_path)) {
    // generate error message
    std::string message{"Target file does not exist: " + file_path};

    // pass message through the log handler
    steamrot::log_handler::ProcessLog(
        spdlog::level::err, steamrot::log_handler::LogCode::kFileNotFound,
        message);
  }
};

////////////////////////////////////////////////////////////
nlohmann::json DataManager::LoadJsonData(const std::string &file_path) {

  // check file exists, this is a go/no go checkpoint
  CheckFileExists(file_path);

  // load json data from file
  std::ifstream file(file_path);

  // return json object
  return nlohmann::json::parse(file);
}
