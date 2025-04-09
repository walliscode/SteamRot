#include "SceneFactory.h"
#include <memory>

std::shared_ptr<SceneMenu> SceneFactory::CreateMenuScene(std::string name) {
  // create a new menu scene object
  return std::make_shared<SceneMenu>(name, 100);
}
