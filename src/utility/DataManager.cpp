#include "DataManager.h"
#include "SchemaChecker.h"
#include <expected>
////////////////////////////////////////////////////////////
void DataManager::LoadData() {};

////////////////////////////////////////////////////////////
EntityConfigData DataManager::LoadSceneDataFromJson(nlohmann::json scene_data) {

  // check json is valid with schema checker
  SchemaChecker schema_checker(SchemaType::kSceneSchema);
  bool proceed = schema_checker.CheckJSON(scene_data);

  return EntityConfigData(scene_data);
}
