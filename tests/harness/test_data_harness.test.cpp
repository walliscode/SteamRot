/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for test data harness utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_harness.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("load_test_data_configs loads from adjacent data directory",
          "[unit][harness]") {
  
  auto result = steamrot::tests::load_test_data_configs();
  
  REQUIRE(result.has_value());
  const auto &configs = result.value();
  
  // Should have at least our sample test data files
  REQUIRE(configs.size() >= 3);
  
  // Verify each config has valid metadata
  for (const auto *config : configs) {
    REQUIRE(config != nullptr);
    REQUIRE(config->metadata() != nullptr);
    REQUIRE(config->metadata()->test_name() != nullptr);
  }
}

TEST_CASE("load_test_data_configs works with Catch2 generators",
          "[unit][harness]") {
  
  // Load all test data configs
  auto configs_result = steamrot::tests::load_test_data_configs();
  REQUIRE(configs_result.has_value());
  
  // Use Catch2 generator to iterate through configs
  const auto *config = GENERATE_COPY(from_range(configs_result.value()));
  
  REQUIRE(config != nullptr);
  REQUIRE(config->metadata() != nullptr);
  REQUIRE(config->metadata()->test_name() != nullptr);
  
  // Verify expected_to_pass is set
  REQUIRE(config->metadata()->expected_to_pass() == true);
  
  // Verify at least some entity data is present
  bool has_entity_data = (config->start_entity_collection() != nullptr ||
                          config->expected_entity_collection() != nullptr);
  REQUIRE(has_entity_data);
}

TEST_CASE("load_test_data_configs with subdirectory parameter",
          "[unit][harness]") {
  
  SECTION("Valid subdirectory returns results") {
    auto result = steamrot::tests::load_test_data_configs("harness");
    REQUIRE(result.has_value());
    REQUIRE(result.value().size() >= 3);
  }
  
  SECTION("Non-existent subdirectory returns error") {
    auto result = steamrot::tests::load_test_data_configs("non_existent_subdir_xyz");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == steamrot::FailMode::FileNotFound);
  }
}

TEST_CASE("Test data harness demonstrates simple workflow",
          "[unit][harness]") {
  
  // Simple one-line call to load all test data
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Use with Catch2 generator for parameterized testing
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Process the test data
  INFO("Test name: " << config->metadata()->test_name()->str());
  
  if (config->metadata()->description()) {
    INFO("Description: " << config->metadata()->description()->str());
  }
  
  // Verify the config has expected structure
  REQUIRE(config->metadata()->version() >= 1);
}
