/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for test harness utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_harness.h"
#include "test_data_loader.h"
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
}

TEST_CASE("Test data harness demonstrates simple workflow", "[unit][harness]") {

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

TEST_CASE("RunTestHarness rejects null config", "[unit][harness]") {

  auto result = steamrot::tests::RunTestHarness(nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("Metadata-only test data loads successfully", "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Find the metadata_validation test config
  const steamrot::TestDataConfig *metadata_config = nullptr;
  for (const auto *config : configs.value()) {
    if (config->metadata()->test_name()->str() == "metadata_validation") {
      metadata_config = config;
      break;
    }
  }

  REQUIRE(metadata_config != nullptr);

  // Verify metadata fields are correctly loaded
  REQUIRE(metadata_config->metadata()->test_name()->str() ==
          "metadata_validation");
  REQUIRE(metadata_config->metadata()->description() != nullptr);
  REQUIRE(metadata_config->metadata()->description()->str() ==
          "Validates test harness metadata loading without entity collections");
  REQUIRE(metadata_config->metadata()->expected_to_pass() == true);
  REQUIRE(metadata_config->metadata()->version() == 1);

  // Verify optional author field is present
  REQUIRE(metadata_config->metadata()->author() != nullptr);
  REQUIRE(metadata_config->metadata()->author()->str() ==
          "Test Infrastructure Team");

  // Verify tags are correctly loaded
  REQUIRE(metadata_config->metadata()->tags() != nullptr);
  REQUIRE(metadata_config->metadata()->tags()->size() == 3);
  REQUIRE(metadata_config->metadata()->tags()->Get(0)->str() == "unit");
  REQUIRE(metadata_config->metadata()->tags()->Get(1)->str() == "harness");
  REQUIRE(metadata_config->metadata()->tags()->Get(2)->str() ==
          "infrastructure");
}

TEST_CASE("Metadata-only test data has no data collections",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Find the metadata_validation test config
  const steamrot::TestDataConfig *metadata_config = nullptr;
  for (const auto *config : configs.value()) {
    if (config->metadata()->test_name()->str() == "metadata_validation") {
      metadata_config = config;
      break;
    }
  }

  REQUIRE(metadata_config != nullptr);

  // Verify that data collections are not present (null)
  REQUIRE(metadata_config->start_data_collection() == nullptr);
  REQUIRE(metadata_config->expected_data_collection() == nullptr);

  // Verify other optional fields are also not present
  REQUIRE(metadata_config->game_resources() == nullptr);
  REQUIRE(metadata_config->scene_resources() == nullptr);
  REQUIRE(metadata_config->simulation_data() == nullptr);
  REQUIRE(metadata_config->event_sequence() == nullptr);
  REQUIRE(metadata_config->input_sequence() == nullptr);
}

TEST_CASE("Metadata-only test data can be used with generators",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Filter to only metadata-only tests
  std::vector<const steamrot::TestDataConfig *> metadata_only_configs;
  for (const auto *config : configs.value()) {
    if (config->start_data_collection() == nullptr &&
        config->expected_data_collection() == nullptr) {
      metadata_only_configs.push_back(config);
    }
  }

  REQUIRE(metadata_only_configs.size() >= 1);

  // Use generator with metadata-only configs
  const auto *config = GENERATE_COPY(from_range(metadata_only_configs));

  INFO("Test name: " << config->metadata()->test_name()->str());

  // Verify metadata structure
  REQUIRE(config->metadata() != nullptr);
  REQUIRE(config->metadata()->test_name() != nullptr);
  REQUIRE(config->metadata()->version() >= 1);

  // Verify no entity data is present
  REQUIRE(config->start_data_collection() == nullptr);
  REQUIRE(config->expected_data_collection() == nullptr);
}
