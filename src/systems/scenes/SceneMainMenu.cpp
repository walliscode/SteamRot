#include "SceneMainMenu.h"

SceneMainMenu::SceneMainMenu(const std::string& name, size_t poolSize, GameEngine& game)
	: Scene(name, poolSize, game) {
}

void SceneMainMenu::update() {

	// each system gets its updates defined separately to allow for different systems in different orders.
	SceneMainMenu::sRender();
}

void SceneMainMenu::sDoAction(const Action& action) {
	// do action
}