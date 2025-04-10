#include "SceneFactory.h"
#include <memory>

std::shared_ptr<Scene> SceneFactory::CreateScene(std::string name,
                                                 std::string scene_type) {
  // create a new scene object, if scene type does not exist throw a runtime
  // error and exit early
  std::shared_ptr<Scene> new_scene = nullptr;
  if (scene_type == "menu") {
    new_scene = CreateMenuScene(name);
  } else {
    throw std::runtime_error("Scene type not found");
  }
  return new_scene;
}

std::shared_ptr<SceneMenu> SceneFactory::CreateMenuScene(std::string name) {
  // create a new menu scene object
  return std::make_shared<SceneMenu>(name, 100);
}
