/////////////////////////////////////////////////
/// @file
/// @brief unit tests for FlatbuffersConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersConfigurator.h"
#include "TestContext.h"
#include "configuration_helpers.h"
#include "containers.h"
#include "scene_types_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("Data is configured correctly from default data",
          "[FlatbuffersConfigurator]") {

  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Set up EntityMemoryPool objects here so they outlive GENERATE
  size_t pool_size{100};
  steamrot::EntityMemoryPool entity_memory_pool_one;
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      entity_memory_pool_one);

  /////////////////////////////////////////////////
  /// Default value testing
  /////////////////////////////////////////////////

  steamrot::tests::TestEMPIsDefaultConstructed(entity_memory_pool_one);

  // create TextContext object
  steamrot::tests::TestContext text_context;
  // Create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      text_context.GetGameContext().event_handler};

  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_memory_pool_one, steamrot::SceneType::SceneType_TEST);
  REQUIRE(result.has_value() == true);
  /////////////////////////////////////////////////
  /// Post configuration testing
  /////////////////////////////////////////////////

  steamrot::tests::TestConfigurationOfEMPfromDefaultData(
      entity_memory_pool_one, steamrot::SceneType_TEST);
}
