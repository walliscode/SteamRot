#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include "GameEngine.h"
#include "EntityManager.h"



TEST_CASE("GameEngine creates an sfml window", "[GameEngine]") {
	GameEngine game;
	std::cout << "\n";
	std::cout << "***********************Game Engine Tests***********************" << "\n";
	std::cout << "\n";

	sf::RenderWindow* window = game.getWindow();
	
	REQUIRE(window != nullptr);  // Check that the window is created
	REQUIRE(window->isOpen());   // Check that the window is open
}

TEST_CASE("Entity Pool tests", "[EntityPool]") {
	std::cout << "\n";
	std::cout << "**********************Entity Pool Tests*********************" << "\n";
	std::cout << "\n";

	std::shared_ptr<EntityMemoryPool> testPool(new EntityMemoryPool(100));
	std::cout << "Pool created of size: " << std::get<0>(*(*testPool).getData()).size() << "\n";
	REQUIRE(std::get<1>(*(*testPool).getData()).size() == 100);   // Check that a pool has been made of the correct size

	int nextFreeIndex = (*testPool).getNextEntityIndex();
	std::cout << "Next free index: " << nextFreeIndex << "\n";
	REQUIRE(nextFreeIndex == 0);   // Check that the next free position is identified correctly

	std::get<0>(*(*testPool).getData())[nextFreeIndex] = 1;
	std::cout << "Data set as added at " << nextFreeIndex << "\n";
	REQUIRE(std::get<0>(*(*testPool).getData())[nextFreeIndex] == 1);   // Check that the data at the free position has been updated 

	nextFreeIndex = (*testPool).getNextEntityIndex();
	std::cout << "Next free index: " << nextFreeIndex << "\n";
	REQUIRE(nextFreeIndex == 1);   // Check that the new next free position is identified correctly

	(*testPool).refreshEntity(*(*testPool).getData(), 0);
	std::cout << "Data reset at index 0\n";
	std::cout << "Data active at index?: " << std::get<0>(*(*testPool).getData())[nextFreeIndex] << "\n";
	REQUIRE(std::get<0>(*(*testPool).getData())[0] == 0);   // Check that the data at the free position has been updated 

	nextFreeIndex = (*testPool).getNextEntityIndex();
	std::cout << "Next free index: " << nextFreeIndex << "\n";
	REQUIRE(nextFreeIndex == 0);   // Check that the new next free position is identified correctly
}


TEST_CASE("Entity Manager tests", "[EntityManager]") {
	std::cout << "\n";
	std::cout << "**********************Entity Manager Tests*********************" << "\n";
	std::cout << "\n";

	std::shared_ptr<EntityManager> testManager(new EntityManager(100));
	std::cout << "Pool position 59 has active set as: " << (*testManager).getComponent<sf::Uint16>(59) << "\n";
	REQUIRE((*testManager).getComponent<sf::Uint16>(59) == 0); // Check that the default state of active for a component slot is false

	(*testManager).getComponent<sf::Uint16>(59) = 1;
	std::cout << "Active at position 59 set to true\n";
	std::cout << "Pool position 59 has active set as: " << (*testManager).getComponent<sf::Uint16>(59) << "\n";
	REQUIRE((*testManager).getComponent<sf::Uint16>(59) == 1); // Check that the changed state of active for a component slot occurs correctly

	std::cout << "Does pool position 59 have a CTransform component?: " << (*testManager).hasComponent<CTransform>(59) << "\n";
	REQUIRE((*testManager).hasComponent<CTransform>(59) == false); // Check that the default state of a component in an entity is that it does not have one

	size_t addedEntity = (*testManager).addEntity();
	std::cout << "Entity added at position: " << addedEntity << "\n";
	REQUIRE(addedEntity == 0); // Check that a new entity is added at the start of the list
	std::cout << "Pool position " << addedEntity << " has active set as: " << (*testManager).getComponent<sf::Uint16>(addedEntity) << "\n";
	REQUIRE((*testManager).getComponent<sf::Uint16>(addedEntity) == 1); // Check that the changed state of active for an added entity occurs correctly

	(*testManager).removeEntity(0);
	std::cout << "Entity removed from position: " << 0 << "\n";
	std::cout << "Pool position 0 has active set as: " << (*testManager).getComponent<sf::Uint16>(0) << "\n";
	REQUIRE((*testManager).getComponent<sf::Uint16>(addedEntity) == 0); // Check that the changed state of active for a removed entity occurs correctly

}


