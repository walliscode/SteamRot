#include <catch2/catch_test_macros.hpp>
#include "ArchetypeManager.h"
#include <iostream>
#include <memory>




TEST_CASE("ArchetpeManager tests", "[ArchetypeManager]") {
	std::cout << "\n";
	std::cout << "**********************ArchetypeManager Tests*********************" << "\n";
	std::cout << "\n";

	std::cout << "Creating ArchetypeManager to test" << std::endl;
	std::shared_ptr<ArchetypeManager> testManager(new ArchetypeManager());

	//check that entities can be added to the archetype manager
	std::vector<std::string> compComb1 = { "Transform", "Meta" };
	(*testManager).assignArchetype(82, compComb1);
	(*testManager).assignArchetype(72, compComb1);
	(*testManager).assignArchetype(62, compComb1);

	std::vector<std::string> compComb2 = { "Transform", "Meta", "Text" };
	(*testManager).assignArchetype(51, compComb2);
	(*testManager).assignArchetype(31, compComb2);

	std::cout << "Testing number of added archetypes" << std::endl;
	REQUIRE((*testManager).getArchetypes().size() == 2); //Test that 2 archetypes were created for the 5  added entities

	//test exact entity list return function
	std::cout << "Testing groupings of added entities" << std::endl;
	REQUIRE((*testManager).getExactArchetypeEntities(compComb1).size() == 3); //Test the 3 entities in the first archetype are returned

	//test inclusive entity return funciton
	std::cout << "Testing total number of added entities" << std::endl;
	REQUIRE((*(*testManager).getInclusiveArchetypeEntities(compComb1)).size() == 5); //Test all 5 entities across both archetype are returned

	//test reassess entity (aslo tests clear)
	(*testManager).reassessEntity(72, compComb2);
	std::cout << "Testing entity has been moved" << std::endl;
	REQUIRE((*testManager).getExactArchetypeEntities(compComb1).size() == 2); //Test the entity has been removed from the first archetype
	std::cout << "Testing all entities are still accounted for" << std::endl;
	REQUIRE((*(*testManager).getInclusiveArchetypeEntities(compComb1)).size() == 5); //Test all 5 entities across both archetype are still returned
	std::cout << "Testing total number of archetypes remaining" << std::endl;
	REQUIRE((*testManager).getArchetypes().size() == 2); //Test that there are still only 2 archetypes
}
