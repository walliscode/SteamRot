/////////////////////////////////////////////////
/// @file
/// @brief Example demonstrating TestDataLoader usage
///
/// This file shows how to use the Test Data Configuration System
/// in actual tests. It's a demonstration file, not a working test.
/////////////////////////////////////////////////

#include "TestDataLoader.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

/////////////////////////////////////////////////
/// Example 1: Basic test data loading
/////////////////////////////////////////////////
TEST_CASE("Example: Load single test data file", "[example][data-driven]") {
  // Initialize TestDataLoader (automatically uses Test environment)
  steamrot::tests::TestDataLoader loader;
  
  // Load a specific test data file from examples subdirectory
  auto result = loader.LoadTestData("example_entity_configuration", "context");
  
  // Verify the load was successful
  REQUIRE(result.has_value());
  
  // Access the configuration
  const auto* config = result.value();
  
  // Access metadata
  REQUIRE(config->metadata() != nullptr);
  REQUIRE(config->metadata()->test_name()->str() == "example_entity_configuration");
  
  // Check if entity collection is present
  if (config->entity_collection()) {
    const auto* entities = config->entity_collection()->entities();
    REQUIRE(entities != nullptr);
    REQUIRE(entities->size() == 2);  // Example has 2 entities
    
    // Access first entity
    const auto* entity = entities->Get(0);
    REQUIRE(entity->index() == 0);
    
    // Access UI component if present
    if (entity->c_user_interface()) {
      REQUIRE(entity->c_user_interface()->position() != nullptr);
      // Would verify position values here
    }
  }
}

/////////////////////////////////////////////////
/// Example 2: Discovering test data files
/////////////////////////////////////////////////
TEST_CASE("Example: Discover all test data in directory", "[example][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  // Discover all test data files in context directory
  auto discovery_result = loader.DiscoverTestDataFiles("context");
  
  REQUIRE(discovery_result.has_value());
  const auto& test_names = discovery_result.value();
  
  // We know context directory has at least 2 test files
  REQUIRE(test_names.size() >= 2);
  
  // Verify expected files are present
  bool found_entity_config = false;
  bool found_simple = false;
  
  for (const auto& name : test_names) {
    if (name == "example_entity_configuration") {
      found_entity_config = true;
    }
    if (name == "simple_metadata_only") {
      found_simple = true;
    }
  }
  
  REQUIRE(found_entity_config);
  REQUIRE(found_simple);
}

/////////////////////////////////////////////////
/// Example 3: Data-driven testing with Catch2 generators
/////////////////////////////////////////////////
TEST_CASE("Example: Data-driven test using generator", "[example][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  // Discover all test data
  auto discovery_result = loader.DiscoverTestDataFiles("context");
  REQUIRE(discovery_result.has_value());
  
  // Use GENERATE_COPY to run test for each discovered file
  auto test_name = GENERATE_COPY(from_range(discovery_result.value()));
  
  SECTION(test_name) {
    // Load the configuration
    auto config_result = loader.LoadTestData(test_name, "context");
    REQUIRE(config_result.has_value());
    
    const auto* config = config_result.value();
    
    // Every test data must have metadata
    REQUIRE(config->metadata() != nullptr);
    REQUIRE(config->metadata()->test_name() != nullptr);
    
    // Verify test name matches
    REQUIRE(config->metadata()->test_name()->str() == test_name);
    
    // Check if test is expected to pass (from metadata)
    bool expected_to_pass = config->metadata()->expected_to_pass();
    
    // Use metadata to control test behavior
    if (expected_to_pass) {
      // Run validation that should succeed
      REQUIRE(config->metadata()->version() > 0);
    }
  }
}

/////////////////////////////////////////////////
/// Example 4: Loading multiple test data files
/////////////////////////////////////////////////
TEST_CASE("Example: Load multiple test data files at once", "[example][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  // List of test names to load
  std::vector<std::string> test_names = {
    "example_entity_configuration",
    "simple_metadata_only"
  };
  
  // Load all configurations
  auto result = loader.LoadMultipleTestData(test_names, "context");
  
  REQUIRE(result.has_value());
  const auto& configs = result.value();
  
  // Verify we got both configurations
  REQUIRE(configs.size() == 2);
  
  // Process each configuration
  for (const auto* config : configs) {
    REQUIRE(config != nullptr);
    REQUIRE(config->metadata() != nullptr);
    
    // Each config can have different data present
    // This demonstrates the extensibility
    std::string name = config->metadata()->test_name()->str();
    
    if (name == "example_entity_configuration") {
      // This one has entity data
      REQUIRE(config->entity_collection() != nullptr);
    } else if (name == "simple_metadata_only") {
      // This one has only metadata (entity_collection is optional)
      // This is fine - demonstrates extensibility
      SUCCEED("Metadata-only test data loaded successfully");
    }
  }
}

/////////////////////////////////////////////////
/// Example 5: Using metadata for test control
/////////////////////////////////////////////////
TEST_CASE("Example: Use metadata to control test behavior", "[example][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  auto result = loader.LoadTestData("example_entity_configuration", "context");
  REQUIRE(result.has_value());
  
  const auto* config = result.value();
  const auto* metadata = config->metadata();
  
  // Access all metadata fields
  REQUIRE(metadata->test_name() != nullptr);
  
  // Optional description
  if (metadata->description()) {
    // Description is present, can log it or use it
    std::string desc = metadata->description()->str();
    REQUIRE(!desc.empty());
  }
  
  // Tags for filtering
  if (metadata->tags()) {
    const auto* tags = metadata->tags();
    bool has_unit_tag = false;
    
    for (size_t i = 0; i < tags->size(); ++i) {
      if (tags->Get(i)->str() == "unit") {
        has_unit_tag = true;
      }
    }
    
    // Could use tags to control test execution
    if (has_unit_tag) {
      // Run unit test validations
      SUCCEED("Unit test tag found");
    }
  }
  
  // Expected outcome
  bool should_pass = metadata->expected_to_pass();
  REQUIRE(should_pass == true);  // Example is expected to pass
  
  // Version tracking
  uint32_t version = metadata->version();
  REQUIRE(version >= 1);
  
  // Optional author
  if (metadata->author()) {
    std::string author = metadata->author()->str();
    // Could use for documentation or reporting
    SUCCEED("Author information present");
  }
}

/////////////////////////////////////////////////
/// Example 6: Error handling
/////////////////////////////////////////////////
TEST_CASE("Example: Handle errors when loading test data", "[example][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  // Try to load a non-existent file
  auto result = loader.LoadTestData("nonexistent_test", "context");
  
  // Should fail gracefully
  REQUIRE_FALSE(result.has_value());
  
  // Can examine the error
  const auto& error = result.error();
  REQUIRE(error.fail_mode == steamrot::FailMode::FileNotFound);
  
  // Error message should be informative
  REQUIRE(!error.message.empty());
}

/////////////////////////////////////////////////
/// Example 7: Testing with different subdirectories
/////////////////////////////////////////////////
TEST_CASE("Example: Organize test data by subdirectory", "[example][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  // Load from examples subdirectory
  auto result = loader.LoadTestData("simple_metadata_only", "context");
  REQUIRE(result.has_value());
  
  // Future: could have component-specific test data
  // auto component_result = loader.LoadTestData("my_component_test", "components");
  
  // Future: could have logic-specific test data
  // auto logic_result = loader.LoadTestData("my_logic_test", "logic");
  
  // This demonstrates the organizational structure
  SUCCEED("Test data organized by subdirectory");
}
