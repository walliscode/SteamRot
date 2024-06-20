#include "Scene.h"

class SceneMainMenu : public Scene {
	

public:
	SceneMainMenu(const std::string& name, size_t poolSize, GameEngine& game);

	void update();
	
};