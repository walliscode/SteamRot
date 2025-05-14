#include "SceneMenu.h"
#include "Scene.h"
#include <cstddef>

SceneMenu::SceneMenu(const std::string &name, size_t pool_size,
                     const EntityConfigData &config_data)
    : Scene(name, pool_size, config_data) {}

void SceneMenu::sMovement() {};

SceneDrawables SceneMenu::sProvideDrawables() {
  SceneDrawables drawables;

  return drawables;
}
