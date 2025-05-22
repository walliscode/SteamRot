////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "SceneFactory.h"
#include "log_handler.h"
#include "spdlog/common.h"
#include "uuid.h"
#include <memory>
#include <string>

namespace steamrot {
////////////////////////////////////////////////////////////
SceneFactory::SceneFactory() {}

////////////////////////////////////////////////////////////
const uuids::uuid SceneFactory::CreateUUID(json scene_data) {

  // check if the scene_data has a uuid
  if (scene_data.contains("uuid")) {

    // get from json
    std::string uuid_string = scene_data["uuid"].get<std::string>();

    // check if the uuid is valid
    if (uuids::uuid::is_valid_uuid(uuid_string)) {

      // return uuid and exit function
      return uuids::uuid::from_string(uuid_string).value();
    }
  }

  // taken straight from the uuid library example
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};

  uuids::uuid const id = gen();

  return id;
}
////////////////////////////////////////////////////////////
std::shared_ptr<Scene> SceneFactory::CreateScene(std::string scene_type,
                                                 json scene_data) {

  // use string scene_type to determine which scene to create
  if (scene_type == "menu") {
    return CreateMenuScene(name);
  } else {

    // this will throw an error if the scene type is not valid
    log_handler::ProcessLog(spdlog::level::level_enum::err,
                            log_handler::LogCode::kInvalidStringParamter,
                            "Invalid scene type: " + scene_type);
  }
}

std::shared_ptr<MenuScene> SceneFactory::CreateMenuScene(std::string name) {
  // get the entity config data from the data manager
  json config_data;

  // create a new menu scene object
  return std::make_shared<MenuScene>(name, 100, config_data);
}

} // namespace steamrot
