/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for entity memory functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entity_memory.h"
#include "CMeta.h"
#include "catch2/catch_test_macros.hpp"

TEST_CASE("ResizeEntityMemoryPool resizes component vectors correctly",
          "[entity_memory]") {
  using namespace steamrot::entity::memory;
  /////////////////////////////////////////////////
  /// @brief create an EntityMemoryPool and test
  /// ResizeEntityMemoryPool
  /////////////////////////////////////////////////
  steamrot::EntityMemoryPool entity_memory_pool;
  auto &cmeta_vec = GetComponentVector<steamrot::CMeta>(entity_memory_pool);
  REQUIRE(cmeta_vec.size() == 0);

  const size_t new_size = 10;
  ResizeEntityMemoryPool(entity_memory_pool, new_size);
  REQUIRE(cmeta_vec.size() == new_size);
}

TEST_CASE("GetEntityMemoryPoolSize returns correct size", "[entity_memory]") {
  using namespace steamrot::entity::memory;
  /////////////////////////////////////////////////
  /// @brief create an EntityMemoryPool and test
  /// GetEntityMemoryPoolSize
  /////////////////////////////////////////////////
  steamrot::EntityMemoryPool entity_memory_pool;
  auto &cmeta_vec = GetComponentVector<steamrot::CMeta>(entity_memory_pool);
  REQUIRE(cmeta_vec.size() == 0);
  const size_t new_size = 15;
  ResizeEntityMemoryPool(entity_memory_pool, new_size);
  REQUIRE(GetMemoryPoolSize(entity_memory_pool) == new_size);
}
TEST_CASE("GetComponentVector returns correct component vector",
          "[entity_memory]") {

  using namespace steamrot::entity::memory;

  /////////////////////////////////////////////////
  /// @brief create an EntityMemoryPool and test GetComponentVector
  /////////////////////////////////////////////////
  steamrot::EntityMemoryPool entity_memory_pool;

  auto &cmeta_vec = GetComponentVector<steamrot::CMeta>(entity_memory_pool);
  REQUIRE(cmeta_vec.size() == 0);
  REQUIRE(std::is_same_v<decltype(cmeta_vec), std::vector<steamrot::CMeta> &>);
}

TEST_CASE("GetComponent returns correct component type", "[entity_memory]") {
  using namespace steamrot::entity::memory;
  /////////////////////////////////////////////////
  /// @brief create an EntityMemoryPool and test GetComponent
  /////////////////////////////////////////////////
  steamrot::EntityMemoryPool entity_memory_pool;
  const size_t new_size = 5;
  ResizeEntityMemoryPool(entity_memory_pool, new_size);
  for (size_t i = 0; i < new_size; ++i) {
    auto &cmeta = GetComponent<steamrot::CMeta>(i, entity_memory_pool);
    REQUIRE(std::is_same_v<decltype(cmeta), steamrot::CMeta &>);
  }
}
