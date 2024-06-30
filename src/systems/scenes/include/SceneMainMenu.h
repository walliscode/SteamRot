#include "Scene.h"

class SceneMainMenu : public Scene {
private:
	void sDoAction(const Action& action); //do action

public:
	SceneMainMenu(const std::string& name, size_t poolSize, GameEngine& game);

	void update();
	
};