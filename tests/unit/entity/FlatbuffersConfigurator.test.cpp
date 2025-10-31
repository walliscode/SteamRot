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
#include <algorithm>
#include <vector>
#include <string>

TEST_CASE("Data is configured correctly from default data",
          "[unit][FlatbuffersConfigurator]") {

  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  
  // Load test data configs from adjacent data directory
  auto configs_result = steamrot::tests::load_test_data_configs();
  REQUIRE(configs_result.has_value());
  
  const auto &configs = configs_result.value();
  
  // Define expected test data names to ensure all tests are run
  std::vector<std::string> expected_test_names = {
    "configurator_basic",
    "pool_comparison_equal",
    "pool_comparison_different_size",
    "pool_comparison_different_values"
  };
  
  // Collect actual test names from loaded configs
  std::vector<std::string> actual_test_names;
  for (const auto *config : configs) {
    if (config->metadata() && config->metadata()->test_name()) {
      actual_test_names.push_back(config->metadata()->test_name()->str());
    }
  }
  
  // Verify all expected tests are present
  for (const auto &expected_name : expected_test_names) {
    INFO("Checking for test: " << expected_name);
    REQUIRE(std::find(actual_test_names.begin(), actual_test_names.end(), 
                     expected_name) != actual_test_names.end());
  }
  
  // Use Catch2 generator to run test for each config
  const auto *config = GENERATE_COPY(from_range(configs));
  
  REQUIRE(config != nullptr);
  REQUIRE(config->metadata() != nullptr);
  
  INFO("Running test: " << config->metadata()->test_name()->str());
  
  // Special handling for configurator_basic test - needs to load default data
  if (config->metadata()->test_name()->str() == "configurator_basic") {
    // Create fixture from test data (starts with empty pool)
    auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);
    REQUIRE(fixture_result.has_value());
    
    auto &fixture = fixture_result.value();
    auto &entity_pool = fixture.GetEntityManager().GetEntityMemoryPool();
    
    // Create configurator and load default scene data
    steamrot::FlatbuffersConfigurator configurator{
        fixture.GetGameContext().event_handler};
    
    auto config_result = configurator.ConfigureEntitiesFromDefaultData(
        entity_pool, steamrot::SceneType::SceneType_TEST);
    REQUIRE(config_result.has_value());
    
    // Create expected pool from test data's expected_entity_collection
    if (config->expected_entity_collection()) {
      auto expected_fixture_result = 
          steamrot::tests::create_fixture_from_test_data(config);
      REQUIRE(expected_fixture_result.has_value());
      
      auto &expected_fixture = expected_fixture_result.value();
      auto &expected_pool = expected_fixture.GetEntityManager().GetEntityMemoryPool();
      
      // Load default data into expected pool too
      auto expected_config_result = configurator.ConfigureEntitiesFromDefaultData(
          expected_pool, steamrot::SceneType::SceneType_TEST);
      REQUIRE(expected_config_result.has_value());
      
      // Compare the pools
      steamrot::tests::run_entity_memory_pool_comparison_test(
          entity_pool, expected_pool,
          config->metadata()->test_name()->str(),
          config->metadata()->expected_to_pass());
    }
  } else {
    // For other tests (pool comparison tests), use standard fixture test
    auto result = steamrot::tests::run_fixture_test(config);
    REQUIRE(result.has_value());
  }
}
