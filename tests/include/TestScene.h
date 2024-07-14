#include "Scene.h"

class TestScene : public Scene {
private:
	void sDoAction(const Action& action) {
		// do action
	}
public:
	TestScene(const std::string& name, int poolSize, GameEngine& game) : Scene(name, poolSize, game) {};
	void update() {
		// put systems here
	}
};