/////////////////////////////////////////////////
/// @file
/// @brief Example demonstrating practical usage of test data generators in a real test scenario
///
/// This file shows how test data generators can be used in actual test files
/// to avoid manual test data management and enable data-driven testing.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_generator.h"
#include "TestDataLoader.h"
#include "FlatbuffersConfigurator.h"
#include "TestContext.h"
#include "entity_test_helpers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

/////////////////////////////////////////////////
/// Example 1: Using generators for simple validation tests
/////////////////////////////////////////////////
TEST_CASE("EXAMPLE: Validate metadata for all test data files",
          "[unit][data_generators][example]") {
  
  // Get all test names from the adjacent data directory
  auto test_names_result = steamrot::tests::get_test_names_for_generator();
  REQUIRE(test_names_result.has_value());
  
  // Generate a test case for each test name
  auto test_name = GENERATE_COPY(from_range(test_names_result.value()));
  
  DYNAMIC_SECTION("Validating: " << test_name) {
    // Load the specific test data
    steamrot::tests::TestDataLoader loader;
    auto config = loader.LoadTestData(test_name, "data_generators");
    REQUIRE(config.has_value());
    
    const auto *test_config = config.value();
    
    // Validate metadata is present and well-formed
    REQUIRE(test_config->metadata() != nullptr);
    REQUIRE(test_config->metadata()->test_name() != nullptr);
    REQUIRE(test_config->metadata()->test_name()->str() == test_name);
    REQUIRE(test_config->metadata()->version() >= 1);
  }
}

/////////////////////////////////////////////////
/// Example 2: Using generators with entity memory pool tests
/////////////////////////////////////////////////
TEST_CASE("EXAMPLE: Process entity collections from test data",
          "[unit][data_generators][example]") {
  
  // Load all test data configs
  auto configs_result = steamrot::tests::load_test_data_for_generator();
  REQUIRE(configs_result.has_value());
  
  // Generate a test case for each config
  const auto *config = GENERATE_COPY(from_range(configs_result.value()));
  
  DYNAMIC_SECTION("Processing: " << config->metadata()->test_name()->str()) {
    // Example: Check if entity collections are present
    bool has_entity_data = (config->entity_collection() != nullptr);
    bool has_start_data = (config->start_entity_collection() != nullptr);
    bool has_expected_data = (config->expected_entity_collection() != nullptr);
    
    // At least one type of entity data should be present
    REQUIRE((has_entity_data || has_start_data || has_expected_data));
    
    // If this were a real test, we could:
    // 1. Create EntityMemoryPool from the data
    // 2. Configure it using FlatbuffersConfigurator
    // 3. Run assertions on the configured pool
    // 4. Compare with expected data if present
  }
}

/////////////////////////////////////////////////
/// Example 3: Demonstrating how this would work with RunEMPComparisonTest
/////////////////////////////////////////////////
TEST_CASE("EXAMPLE: Integration with entity test helpers",
          "[unit][data_generators][example]") {
  
  // This example shows how test data generators would be used with
  // the existing entity test helper functions
  
  auto test_names_result = steamrot::tests::get_test_names_for_generator();
  REQUIRE(test_names_result.has_value());
  
  auto test_name = GENERATE_COPY(from_range(test_names_result.value()));
  
  DYNAMIC_SECTION("Testing with: " << test_name) {
    // Load test data
    steamrot::tests::TestDataLoader loader;
    auto config = loader.LoadTestData(test_name, "data_generators");
    REQUIRE(config.has_value());
    
    const auto *test_config = config.value();
    
    // In a real scenario with entity tests, you would:
    // steamrot::tests::TestContext test_context;
    // steamrot::FlatbuffersConfigurator configurator{
    //     test_context.GetGameContext().event_handler};
    // 
    // steamrot::tests::RunEMPComparisonTest(test_config, configurator);
    
    // For this example, we just verify the structure
    if (test_config->start_entity_collection() && 
        test_config->expected_entity_collection()) {
      INFO("This is a comparison test");
      REQUIRE(test_config->metadata()->expected_to_pass());
    } else if (test_config->entity_collection()) {
      INFO("This is a simple entity collection test");
      REQUIRE(test_config->entity_collection()->entities() != nullptr);
    }
  }
}

/////////////////////////////////////////////////
/// Example 4: Pattern for filtering tests by tags
/////////////////////////////////////////////////
TEST_CASE("EXAMPLE: Filter tests by metadata tags",
          "[unit][data_generators][example]") {
  
  // Load all configs
  auto configs_result = steamrot::tests::load_test_data_for_generator();
  REQUIRE(configs_result.has_value());
  
  // Filter to only configs with "multiple" tag
  std::vector<const steamrot::TestDataConfig *> filtered_configs;
  for (const auto *config : configs_result.value()) {
    if (config->metadata()->tags()) {
      auto tags = config->metadata()->tags();
      for (size_t i = 0; i < tags->size(); ++i) {
        if (tags->Get(i)->str() == "multiple") {
          filtered_configs.push_back(config);
          break;
        }
      }
    }
  }
  
  // If we have any configs with the "multiple" tag
  if (!filtered_configs.empty()) {
    const auto *config = GENERATE_COPY(from_range(filtered_configs));
    
    DYNAMIC_SECTION("Tagged test: " << config->metadata()->test_name()->str()) {
      // Process only tests with "multiple" tag
      REQUIRE(config->start_entity_collection() != nullptr);
      REQUIRE(config->expected_entity_collection() != nullptr);
    }
  } else {
    SUCCEED("No tests with 'multiple' tag found");
  }
}

/////////////////////////////////////////////////
/// Example 5: Pattern for organizing multiple test suites
/////////////////////////////////////////////////
TEST_CASE("EXAMPLE: Separate test suites from same data source",
          "[unit][data_generators][example]") {
  
  auto test_names_result = steamrot::tests::get_test_names_for_generator();
  REQUIRE(test_names_result.has_value());
  
  auto test_name = GENERATE_COPY(from_range(test_names_result.value()));
  
  steamrot::tests::TestDataLoader loader;
  auto config = loader.LoadTestData(test_name, "data_generators");
  REQUIRE(config.has_value());
  
  const auto *test_config = config.value();
  
  SECTION("Metadata validation suite") {
    REQUIRE(test_config->metadata() != nullptr);
    REQUIRE(test_config->metadata()->version() >= 1);
  }
  
  SECTION("Entity data validation suite") {
    bool has_entity_data = (test_config->entity_collection() != nullptr ||
                            test_config->start_entity_collection() != nullptr ||
                            test_config->expected_entity_collection() != nullptr);
    REQUIRE(has_entity_data);
  }
  
  SECTION("Expected outcome validation") {
    // Verify expected_to_pass matches the actual test expectation
    REQUIRE(test_config->metadata()->expected_to_pass());
  }
}
