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
	std::cout << "**********************Entity Manager Tests*********************" << "\n";
	std::cout << "\n";
	std::shared_ptr<EntityMemoryPool> testPool(new EntityMemoryPool(100));
	std::cout << "testPool size: " << std::get<0>(*(*testPool).getData()).size() << "\n";
	std::cout << "Next free index: " << (*testPool).getNextEntityIndex() << "\n";
}
