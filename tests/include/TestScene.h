#include "Scene.h"

class TestScene : public Scene {

public:
	TestScene(const std::string& name, int poolSize, GameEngine& game) : Scene(name, poolSize, game) {};
	void update() {
		// put systems here
	}
};