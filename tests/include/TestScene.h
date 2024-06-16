#include "Scene.h"

class TestScene : public Scene {

public:
	TestScene(const std::string& name, int poolSize) : Scene(name, poolSize) {};
	void update() {
		// put systems here
	}
};