/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersLogicConfigCollectionProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersLogicConfigCollectionProvider.h"
#include "LogicType.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE(
    "FlatbuffersLogicConfigCollectionProvider::CreateLogicConfigCollection "
    "returns a configured LogicConfigCollection object",
    "[FlatbuffersLogicConfigCollectionProvider]") {
  steamrot::FlatbuffersLogicConfigCollectionProvider provider;
  auto result = provider.CreateLogicConfigCollection();

  if (!result) {
    FAIL("CreateLogicConfigCollection failed with error: " +
         result.error().message);
  }

  steamrot::LogicConfigCollection &collection = result.value();

  REQUIRE(!collection.empty());
  REQUIRE(collection.contains(steamrot::LogicType::TestLogic));
}
