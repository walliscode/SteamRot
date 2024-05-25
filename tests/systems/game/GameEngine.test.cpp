#include <catch2/catch_test_macros.hpp>
#include "GameEngine.h"
#include "Entity.h"



TEST_CASE("GameEngine creates an sfml window", "[GameEngine]") {
	GameEngine game;

	sf::RenderWindow* window = game.getWindow();
	
	REQUIRE(window != nullptr);  // Check that the window is created
	REQUIRE(window->isOpen());   // Check that the window is open
}

TEST_CASE("Entity can be made the id called", "[Entity]") {
	Entity entity(1000);

	REQUIRE(entity.id() == 1000); // Check that the entity exists and has an ID

}