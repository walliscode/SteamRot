#include "SceneMenu.h"
#include "Scene.h"
#include <cstddef>

namespace steamrot {

SceneMenu::SceneMenu(const std::string &name, size_t pool_size,
                     const json &config_data)
    : Scene(name, pool_size, config_data) {}

void SceneMenu::sMovement() {};

SceneDrawables SceneMenu::sProvideDrawables() {
  SceneDrawables drawables;

  return drawables;
}
} // namespace steamrot
