/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the EntityManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityManager.h"
#include "EntityMemoryPoolEqualsMatcher.h"
#include "PathProvider.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("EntityManager calls configurator with no errors",
          "[unit][EntityManager]") {

  // create text context
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestFixture test_context;
  steamrot::EntityManager entity_manager{
      test_context.GetGameContext().event_handler};

  // test the EntityMemoryPool pre configuration by comparing to default
  steamrot::EntityMemoryPool expected_default_pool;
  REQUIRE_THAT(entity_manager.GetEntityMemoryPool(),
               steamrot::tests::EqualsEntityMemoryPool(expected_default_pool));

  // configure entities from default data
  auto result = entity_manager.ConfigureEntitiesFromDefaultData(
      steamrot::SceneType::SceneType_TEST, steamrot::DataType::Flatbuffers);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  // test the EntityMemoryPool post configuration by creating expected pool
  steamrot::EntityManager expected_entity_manager{
      test_context.GetGameContext().event_handler};
  auto expected_result =
      expected_entity_manager.ConfigureEntitiesFromDefaultData(
          steamrot::SceneType::SceneType_TEST, steamrot::DataType::Flatbuffers);
  REQUIRE(expected_result.has_value());

  REQUIRE_THAT(entity_manager.GetEntityMemoryPool(),
               steamrot::tests::EqualsEntityMemoryPool(
                   expected_entity_manager.GetEntityMemoryPool()));
}
