#include <catch2/catch_test_macros.hpp>
#include "EntityManager.h"


TEST_CASE("Entity Manager tests", "[EntityManager]") {
	std::cout << "\n";
	std::cout << "**********************Entity Manager Tests*********************" << "\n";
	std::cout << "\n";

	std::shared_ptr<EntityManager> testManager(new EntityManager("testPool",100));
	std::cout << "Pool position 59 has active set as: " << (*testManager).getComponent<CMeta>(59).getActive() << "\n";
	REQUIRE((*testManager).getComponent<CMeta>(59).getActive() == false); // Check that the default state of active for a component slot is false

	(*testManager).getComponent<CMeta>(59).activate();
	std::cout << "m_active at position 59 set to true\n";
	std::cout << "Pool position 59 has active set as: " << (*testManager).getComponent<CMeta>(59).getActive() << "\n";
	REQUIRE((*testManager).getComponent<CMeta>(59).getActive() == true); // Check that the changed state of active for a component slot occurs correctly

	std::cout << "Does pool position 59 have a CTransform component?: " << (*testManager).hasComponent<CTransform>(59) << "\n";
	REQUIRE((*testManager).hasComponent<CTransform>(59) == false); // Check that the default state of a component in an entity is that it does not have one

	size_t addedEntity = (*testManager).addEntity();
	std::cout << "Entity added at position: " << addedEntity << "\n";
	REQUIRE(addedEntity == 0); // Check that a new entity is added at the start of the list
	std::cout << "Pool position " << addedEntity << " has active set as: " << (*testManager).getComponent<CMeta>(addedEntity).getActive() << "\n";
	REQUIRE((*testManager).getComponent<CMeta>(addedEntity).getActive() == true); // Check that the changed state of active for an added entity occurs correctly
	std::cout << "Entities in active list: " << (*testManager).getEntities().size() << "\n";
	REQUIRE((*testManager).getEntities().size() == 0); // Check that the active entities list is empty
	(*testManager).updateWaitingRooms();
	std::cout << "Waiting rooms updated\n";
	std::cout << "Entities in active list: " << (*testManager).getEntities().size() << "\n";
	REQUIRE((*testManager).getEntities().size() == 1); // Check that the active entities list is not empty now


	(*testManager).removeEntity(0);
	std::cout << "Entity removed from position: " << 0 << "\n";
	std::cout << "Pool position 0 has active set as: " << (*testManager).getComponent<CMeta>(0).getActive() << "\n";
	REQUIRE((*testManager).getComponent<CMeta>(addedEntity).getActive() == true); // Check that the changed state of active for a removed entity has not happened yet
	std::cout << "Entities in active list: " << (*testManager).getEntities().size() << "\n";
	REQUIRE((*testManager).getEntities().size() == 1); // Check that the active entities list is not empty now
	(*testManager).updateWaitingRooms();
	std::cout << "Waiting rooms updated\n";
	std::cout << "Entities in active list: " << (*testManager).getEntities().size() << "\n";
	REQUIRE((*testManager).getEntities().size() == 0); // Check that the active entities list is not empty now
	std::cout << "Pool position 0 has active set as: " << (*testManager).getComponent<CMeta>(0).getActive() << "\n";
	REQUIRE((*testManager).getComponent<CMeta>(addedEntity).getActive() == false); // Check that the changed state of active for a removed entity has now updated
}