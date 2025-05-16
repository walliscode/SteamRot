////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "SceneFactory.h"
#include "log_handler.h"
#include "spdlog/common.h"
#include <memory>
#include <string>

namespace steamrot {
////////////////////////////////////////////////////////////
SceneFactory::SceneFactory(std::shared_ptr<DataManager> data_manager)
    : m_data_manager(data_manager) {}

////////////////////////////////////////////////////////////
std::shared_ptr<Scene> SceneFactory::CreateScene(std::string name,
                                                 std::string scene_type) {

  // create place holder Scene object
  std::shared_ptr<Scene> new_scene = nullptr;

  // use string scene_type to determine which scene to create
  if (scene_type == "menu") {
    new_scene = CreateMenuScene(name);
  } else {

    // this will throw an error if the scene type is not valid
    log_handler::ProcessLog(spdlog::level::level_enum::err,
                            log_handler::LogCode::kInvalidStringParamter,
                            "Invalid scene type: " + scene_type);
  }
  return new_scene;
}

std::shared_ptr<SceneMenu> SceneFactory::CreateMenuScene(std::string name) {
  // get the entity config data from the data manager
  json config_data;

  // create a new menu scene object
  return std::make_shared<SceneMenu>(name, 100, config_data);
}

} // namespace steamrot
