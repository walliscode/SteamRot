#include <catch2/catch_test_macros.hpp>
#include "Container.h"
#include "ContainerManager.h"


// units test for ContainerManager using Catch2

TEST_CASE("ContainerManager test", "[ContainerManager]") {

	// intialize a container manager for unit testing
	ContainerManager containerManager;
	// check that "root" container is created
	REQUIRE(containerManager.getContainer("root")->getTag() == "root");
	// check that a new container can be created with root as parent
	containerManager.createContainer("test", "root");
	REQUIRE(containerManager.getContainer("test")->getTag() == "test");
	// check that parent container has a pointer to the new container
	REQUIRE(containerManager.getContainer("root")->getChildrenContainers().size() == 1);

	std::shared_ptr<Container> testContainer = containerManager.getContainer("test");
	std::shared_ptr<Container> rootTestContainer = containerManager.getContainer("root")->getChildrenContainers()[0];

	REQUIRE(testContainer == rootTestContainer);
}
