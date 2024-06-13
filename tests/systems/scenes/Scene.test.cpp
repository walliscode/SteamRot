#include <catch2/catch_test_macros.hpp>
#include "Scene.h"

class TestScene : public Scene {

public:
	TestScene(int poolSize) : Scene(poolSize) {};
	void update() {
		// put systems here
	}
};

TEST_CASE("Scene is created with an entity manager", "[Scene]") {
	TestScene TestScene(10);

	REQUIRE(TestScene.getActive() == true); // check that it intialises with active scene
}