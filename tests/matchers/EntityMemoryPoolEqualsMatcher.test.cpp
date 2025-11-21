/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EntityMemoryPoolEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityMemoryPoolEqualsMatcher.h"
#include "conmat.h"
#include "emp_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EntityMemoryPoolEqualsMatcher works correctly",
          "[unit][EntityMemoryPool][matcher]") {
  const size_t pool_size = 5;
  steamrot::EntityMemoryPool expected =
      steamrot::entity::memory::CreateEntityMemoryPool(pool_size);
  steamrot::EntityMemoryPool actual =
      steamrot::entity::memory::CreateEntityMemoryPool(pool_size);

  SECTION("Matcher detects equality for same size pools") {
    REQUIRE_THAT(actual, steamrot::tests::EqualsEntityMemoryPool(expected));
  }

  SECTION("Matcher detects differences in pool size") {
    steamrot::EntityMemoryPool different_pool =
        steamrot::entity::memory::CreateEntityMemoryPool(3);
    REQUIRE_THAT(different_pool,
                 !steamrot::tests::EqualsEntityMemoryPool(expected));
  }

  SECTION("Matcher detects differences in component data") {
    // Activate a component in actual but not in expected
    auto &meta = steamrot::emp_helpers::GetComponent<steamrot::CMeta>(0, actual);
    meta.m_active = true;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEntityMemoryPool(expected));
  }
}

TEST_CASE("EntityMemoryPoolEqualsMatcher describe is as expected on success",
          "[unit][EntityMemoryPool][matcher]") {
  const size_t pool_size = 5;
  steamrot::EntityMemoryPool expected =
      steamrot::entity::memory::CreateEntityMemoryPool(pool_size);
  steamrot::EntityMemoryPool actual =
      steamrot::entity::memory::CreateEntityMemoryPool(pool_size);
  auto matcher = steamrot::tests::EqualsEntityMemoryPool(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("[PASSED]") != std::string::npos);
  REQUIRE(description.find("EntityMemoryPool Match") != std::string::npos);
}

TEST_CASE("EntityMemoryPoolEqualsMatcher describe is as expected on failure",
          "[unit][EntityMemoryPool][matcher]") {
  const size_t expected_size = 5;
  const size_t actual_size = 3;
  steamrot::EntityMemoryPool expected =
      steamrot::entity::memory::CreateEntityMemoryPool(expected_size);
  steamrot::EntityMemoryPool actual =
      steamrot::entity::memory::CreateEntityMemoryPool(actual_size);

  auto matcher = steamrot::tests::EqualsEntityMemoryPool(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("[FAILED]") != std::string::npos);
  REQUIRE(description.find("EntityMemoryPool Match") != std::string::npos);
  REQUIRE(description.find("Size Mismatch:") != std::string::npos);
}
