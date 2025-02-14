#include "SceneMenu.h"
#include "Scene.h"

SceneMenu::SceneMenu(const std::string &name, size_t poolSize)
    : Scene(name, poolSize) {}

void SceneMenu::sUpdate() {};

SceneDrawableMap SceneMenu::sProvideDrawables() {
  SceneDrawableMap drawables;
  return drawables;
}
