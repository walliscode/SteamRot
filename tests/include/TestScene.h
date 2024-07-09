#include "Scene.h"

class TestScene : public Scene {
private:
	void sDoAction(const Action& action) {
		// do action
	}
public:
	TestScene(const std::string& name, int poolSize, GameEngine& game, SceneManager& sceneManager) : Scene(name, poolSize, game, sceneManager) {};
	void update() {
		// put systems here
	}
};