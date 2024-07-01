#include <catch2/catch_test_macros.hpp>
#include "Archetype.h"
#include <iostream>
#include <memory>




TEST_CASE("Archetpe tests", "[Archetype]") {
	std::cout << "\n";
	std::cout << "**********************Archetype Tests*********************" << "\n";
	std::cout << "\n";

	std::vector<size_t> testIDs = { 1, 5, 3, 8 };
	auto testArchetype = std::make_shared<Archetype>(testIDs);
	std::cout << "Archetype created with test binary code: " << (*testArchetype).getCode() << "\n";
	REQUIRE(*(*testArchetype).getCode() == 149);   // Check that an archetype has been made with the correct code

	(*testArchetype).addEntity(69);
	(*testArchetype).addEntity(82);
	(*testArchetype).addEntity(14);
	std::cout << "3 entities added, new size: " << (*testArchetype).getEntities().size() << "\n";
	REQUIRE((*testArchetype).getEntities().size() == 3);   // Check that entity IDs can be added to the archetype

	(*testArchetype).removeEntity(82);
	std::cout << "Entity removed, new size: " << (*testArchetype).getEntities().size() << "\n";
	REQUIRE((*testArchetype).getEntities().size() == 2);   // Check that an entity has been successfully removed

}
