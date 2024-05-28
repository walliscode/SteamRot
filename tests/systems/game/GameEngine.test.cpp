#include <catch2/catch_test_macros.hpp>
#include "GameEngine.h"



TEST_CASE("GameEngine creates an sfml window", "[GameEngine]") {
	GameEngine game;

	sf::RenderWindow* window = game.getWindow();
	
	REQUIRE(window != nullptr);  // Check that the window is created
	REQUIRE(window->isOpen());   // Check that the window is open
}
