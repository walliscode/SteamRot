/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the EntityManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityManager.h"
#include "emp_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EntityManager creates various pool sizes", "[EntityManager]") {
  SECTION("Default pool size") {
    steamrot::EntityManager entity_manager{};
    REQUIRE(steamrot::emp_helpers::GetMemoryPoolSize(
                entity_manager.GetEntityMemoryPool()) == 100);
  }
  SECTION("Custom pool size") {
    const size_t custom_size = 156;
    steamrot::EntityManager entity_manager{custom_size};
    REQUIRE(steamrot::emp_helpers::GetMemoryPoolSize(
                entity_manager.GetEntityMemoryPool()) == custom_size);
  }
}
