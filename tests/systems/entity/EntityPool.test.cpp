#include <catch2/catch_test_macros.hpp>
#include "EntityPool.h"

class TestEntityPool : public EntityPool {
public:
	bool m_exists = true;
	TestEntityPool() : EntityPool() {}

	
};

TEST_CASE("EntityPool is created", "[EntityPool]") {
	TestEntityPool ep;

	REQUIRE(ep.m_exists == true); // Check that the entity pool is created
	REQUIRE(ep.m_data); // Check that the entity pool data is created
}
