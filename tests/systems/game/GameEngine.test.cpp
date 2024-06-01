#include <catch2/catch_test_macros.hpp>
#include "GameEngine.h"
#include "EntityManager.h"
#include <iostream>



TEST_CASE("GameEngine creates an sfml window", "[GameEngine]") {
	GameEngine game;
	std::cout << "\n";
	std::cout << "***********************Game Engine Tests***********************" << "\n";
	std::cout << "\n";

	sf::RenderWindow* window = game.getWindow();
	
	REQUIRE(window != nullptr);  // Check that the window is created
	REQUIRE(window->isOpen());   // Check that the window is open
}

TEST_CASE("Entity Manager tests", "[EntityManager]") {
	std::cout << "\n";
	std::cout << "**********************Entity Pool Tests*********************" << "\n";
	std::cout << "\n";

	std::shared_ptr<EntityMemoryPool> testPool(new EntityMemoryPool(100));
	std::cout << "Pool created of size: " << std::get<0>(*(*testPool).getData()).size() << "\n";
	REQUIRE(std::get<1>(*(*testPool).getData()).size() == 100);   // Check that a pool has been made of the correct size

	int nextFreeIndex = (*testPool).getNextEntityIndex();
	std::cout << "Next free index: " << nextFreeIndex << "\n";
	REQUIRE(nextFreeIndex == 0);   // Check that the next free position is identified correctly

	std::get<0>(*(*testPool).getData())[nextFreeIndex] = true;
	std::cout << "Data set as added at " << nextFreeIndex << "\n";
	REQUIRE(std::get<0>(*(*testPool).getData())[nextFreeIndex] == true);   // Check that the data at the free position has been updated 

	nextFreeIndex = (*testPool).getNextEntityIndex();
	std::cout << "Next free index: " << nextFreeIndex << "\n";
	REQUIRE(nextFreeIndex == 1);   // Check that the new next free position is identified correctly

	(*testPool).refreshEntity(*(*testPool).getData(), 0);
	std::cout << "Data reset at index 0\n";
	std::cout << "Data active at index?: " << std::get<0>(*(*testPool).getData())[nextFreeIndex] << "\n";
	REQUIRE(std::get<0>(*(*testPool).getData())[0] == false);   // Check that the data at the free position has been updated 

	nextFreeIndex = (*testPool).getNextEntityIndex();
	std::cout << "Next free index: " << nextFreeIndex << "\n";
	REQUIRE(nextFreeIndex == 0);   // Check that the new next free position is identified correctly
}
