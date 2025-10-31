/////////////////////////////////////////////////
/// @file
/// @brief unit tests for FlatbuffersConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersConfigurator.h"
#include "TestFixture.h"
#include "entity_test_helpers.h"
#include "entity_memory_pool_matchers.h"
#include "containers.h"
#include "scene_change_packet_generated.h"
#include "test_data_harness.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
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
  steamrot::tests::TestFixture text_context;
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

/////////////////////////////////////////////////
/// Data-driven configurator tests using test harness
/////////////////////////////////////////////////

TEST_CASE("Entity configuration from test data (data-driven)",
          "[unit][FlatbuffersConfigurator]") {

  // Load test data configurations from adjacent data directory
  auto configs_result = steamrot::tests::load_test_data_configs();
  REQUIRE(configs_result.has_value());

  // Use Catch2 generator to parameterize test with each config
  const auto *config = GENERATE_COPY(from_range(configs_result.value()));

  SECTION(config->metadata()->test_name()->c_str()) {
    // Run the fixture test which will:
    // 1. Create a TestFixture with start_entity_collection
    // 2. If expected_entity_collection is present, compare results
    // 3. Handle expected_to_pass metadata internally
    auto result = steamrot::tests::run_fixture_test(config);
    
    // The function handles expected_to_pass internally, so we just check
    // that it completed without errors
    REQUIRE(result.has_value());
  }
}
