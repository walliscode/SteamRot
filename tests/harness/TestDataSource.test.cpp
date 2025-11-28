/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for TestDataSource class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestDataSource.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("TestDataSource construction", "[unit][TestDataSource]") {
  SECTION("Can be constructed with null config") {
    TestDataSource source(nullptr);
    REQUIRE(source.GetTestConfig() == nullptr);
  }
}

TEST_CASE("TestDataSource with null config", "[unit][TestDataSource]") {
  TestDataSource source(nullptr);

  SECTION("GetEntityCollection returns nullptr") {
    REQUIRE(source.GetEntityCollection() == nullptr);
  }

  SECTION("GetEntityCount returns 0") { REQUIRE(source.GetEntityCount() == 0); }

  SECTION("GetSourceIdentifier returns base name") {
    REQUIRE(source.GetSourceIdentifier() == "TestDataSource");
  }
}

} // namespace steamrot::tests
