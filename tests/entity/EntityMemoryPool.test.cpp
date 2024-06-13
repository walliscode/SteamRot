#include <catch2/catch_test_macros.hpp>
#include "EntityMemoryPool.h"



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

	std::get<std::vector<CMeta>>(*(*testPool).getData())[nextFreeIndex].activate(); // Set the data at the first free position to active, std::vector is needed with CMeta
	std::cout << "Data set as added at " << nextFreeIndex << "\n";
	REQUIRE(std::get<std::vector<CMeta>>(*(*testPool).getData())[nextFreeIndex].getActive() == true);   // Check that the data at the free position has been updated 

	nextFreeIndex = (*testPool).getNextEntityIndex();
	std::cout << "Next free index: " << nextFreeIndex << "\n";
	REQUIRE(nextFreeIndex == 1);   // Check that the new next free position is identified correctly

	
}
