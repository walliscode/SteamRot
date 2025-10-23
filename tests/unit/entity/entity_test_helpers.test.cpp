/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for entity test helper functions using data-driven approach
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entity_test_helpers.h"
#include "FlatbuffersConfigurator.h"
#include "TestContext.h"
#include "TestDataLoader.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// @brief Helper function to create and configure an EntityMemoryPool from test data
///
/// @param test_data_name Name of the test data file to load
/// @param configurator FlatbuffersConfigurator instance to use
/// @return Configured EntityMemoryPool
/////////////////////////////////////////////////
steamrot::EntityMemoryPool CreatePoolFromTestData(
    const std::string &test_data_name,
    steamrot::FlatbuffersConfigurator &configurator) {
  
  steamrot::tests::TestDataLoader loader;
  
  // Load test data
  auto result = loader.LoadTestData(test_data_name, "unit/entity");
  REQUIRE(result.has_value());
  
  const auto *config = result.value();
  REQUIRE(config->entity_collection() != nullptr);
  
  // Create entity memory pool
  steamrot::EntityMemoryPool pool;
  
  // Configure the pool using FlatbuffersConfigurator's new method
  auto configure_result = configurator.ConfigureEntitiesFromCollection(
      pool, config->entity_collection());
  
  REQUIRE(configure_result.has_value());
  
  return pool;
}

TEST_CASE("CompareEntityMemoryPools detects equal pools",
          "[unit][entity_test_helpers][data-driven]") {

  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestContext test_context;
  
  // Create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      test_context.GetGameContext().event_handler};

  // Create two identical pools from the same test data
  auto pool1 = CreatePoolFromTestData("pool_comparison_equal", configurator);
  auto pool2 = CreatePoolFromTestData("pool_comparison_equal", configurator);

  // This should pass without throwing - both pools are identical
  REQUIRE_NOTHROW(
      steamrot::tests::CompareEntityMemoryPools(pool1, pool2));
}

TEST_CASE("CompareEntityMemoryPools fails for different pool sizes",
          "[unit][entity_test_helpers][data-driven]") {

  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestContext test_context;
  
  // Create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      test_context.GetGameContext().event_handler};

  // Create pools with different sizes from different test data
  auto pool1 = CreatePoolFromTestData("pool_comparison_equal", configurator);
  auto pool2 = CreatePoolFromTestData("pool_comparison_different_size", configurator);

  // This should throw/fail because the pools have different sizes
  REQUIRE_THROWS(
      steamrot::tests::CompareEntityMemoryPools(pool1, pool2));
}

TEST_CASE("CompareEntityMemoryPools fails for different component values",
          "[unit][entity_test_helpers][data-driven]") {

  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestContext test_context;
  
  // Create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      test_context.GetGameContext().event_handler};

  // Create pools with different component values from different test data
  auto pool1 = CreatePoolFromTestData("pool_comparison_equal", configurator);
  auto pool2 = CreatePoolFromTestData("pool_comparison_different_values", configurator);

  // This should throw/fail because component values differ
  REQUIRE_THROWS(
      steamrot::tests::CompareEntityMemoryPools(pool1, pool2));
}
