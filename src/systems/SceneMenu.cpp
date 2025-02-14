#include "SceneMenu.h"

SceneMenu::SceneMenu(const std::string &name, size_t poolSize)
    : Scene(name, poolSize) {}

void SceneMenu::sUpdate() {};

std::vector<std::shared_ptr<sf::Drawable>> SceneMenu::sProvideDrawables() {
  return std::vector<std::shared_ptr<sf::Drawable>>();
}
