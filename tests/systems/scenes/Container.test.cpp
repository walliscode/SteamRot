#include <catch2/catch_test_macros.hpp>
#include "Container.h"


TEST_CASE("Container") {
	Container container("test");
	REQUIRE(container.getTag() == "test");

	container.setTag("test2");
	REQUIRE(container.getTag() == "test2");

	container.addEntityIndex(1);
	REQUIRE(container.getEntityIndexes().size() == 1);
	REQUIRE(container.getEntityIndexes()[0] == 1);


	sf::Vector2f size(2.0f, 2.0f);
	container.setProportionalSize(size);
	REQUIRE(container.getProportionalSize() == size);

	std::shared_ptr<Container> childContainer = std::make_shared<Container>("child");
	container.addChildContainer(childContainer);
	REQUIRE(container.getChildrenContainers().size() == 1);
	REQUIRE(container.getChildrenContainers()[0] == childContainer);
}
