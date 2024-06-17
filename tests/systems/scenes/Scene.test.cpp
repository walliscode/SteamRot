#include <catch2/catch_test_macros.hpp>
#include "TestScene.h"

TEST_CASE("Scene is created with an entity manager", "[Scene]") {
	TestScene TestScene("SceneTest", 10);

	REQUIRE(TestScene.getActive() == true); // check that it intialises with active scene
}