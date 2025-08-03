/////////////////////////////////////////////////
/// @file
/// @brief unit tests for FlatbuffersConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityHelpers.h"
#include "containers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("The entity memory pool is configured correctly for the test data",
          "[FlatbuffersConfigurator]") {

  // create EntityMemoryPool and resize
  steamrot::components::containers::EntityMemoryPool entity_memory_pool;
  // resize the pool to accommodate 50 entities
  size_t pool_size = 50;
  steamrot::ResizeEntityMemoryPool(entity_memory_pool, pool_size);

  // check the size of the pool
  REQUIRE(steamrot::GetMemoryPoolSize(entity_memory_pool) == pool_size);
}
