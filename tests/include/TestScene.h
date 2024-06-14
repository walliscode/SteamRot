#include "Scene.h"

class TestScene : public Scene {

public:
	TestScene(int poolSize) : Scene(poolSize) {};
	void update() {
		// put systems here
	}
};