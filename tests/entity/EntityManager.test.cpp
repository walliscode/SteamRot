/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the EntityManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityManager.h"
#include "PathProvider.h"
#include "TestContext.h"
#include "configuration_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EntityManager calls configurator with no errors",
          "[EntityManager]") {

  // create text context
  steamrot::tests::TestContext test_context;
  steamrot::EntityManager entity_manager{
      test_context.GetGameContext().event_handler};

  // test the EntityMemoryPool pre configuration
  steamrot::tests::TestEMPIsDefaultConstructed(
      entity_manager.GetEntityMemoryPool());

  // configure entities from default data
  auto result = entity_manager.ConfigureEntitiesFromDefaultData(
      steamrot::SceneType::SceneType_TEST, steamrot::EnvironmentType::Test,
      steamrot::DataType::Flatbuffers);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  // test the EntityMemoryPool post configuration
  steamrot::tests::TestConfigurationOfEMPfromDefaultData(
      entity_manager.GetEntityMemoryPool(),
      steamrot::SceneType::SceneType_TEST);
}
