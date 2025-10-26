/////////////////////////////////////////////////
/// @file
/// @brief unit tests for FlatbuffersConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersConfigurator.h"
#include "TestContext.h"
#include "entity_test_helpers.h"
#include "entity_memory_pool_matchers.h"
#include "containers.h"
#include "scene_change_packet_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("Data is configured correctly from default data",
          "[unit][FlatbuffersConfigurator]") {

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

  // Check that the pool is default constructed by comparing to a new pool
  steamrot::EntityMemoryPool expected_default_pool;
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      expected_default_pool);
  REQUIRE_THAT(entity_memory_pool_one,
               steamrot::tests::EqualsEntityMemoryPool(expected_default_pool));

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

  // Create expected pool configured with the same data
  steamrot::EntityMemoryPool expected_configured_pool;
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      expected_configured_pool);
  auto expected_result = configurator.ConfigureEntitiesFromDefaultData(
      expected_configured_pool, steamrot::SceneType::SceneType_TEST);
  REQUIRE(expected_result.has_value() == true);

  // Compare the pools using matcher
  REQUIRE_THAT(entity_memory_pool_one,
               steamrot::tests::EqualsEntityMemoryPool(expected_configured_pool));
}
