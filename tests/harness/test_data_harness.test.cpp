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
    auto result =
        steamrot::tests::load_test_data_configs("non_existent_subdir_xyz");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == steamrot::FailMode::FileNotFound);
  }
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

TEST_CASE("run_test_data_config validates null config", "[unit][harness]") {

  auto result = steamrot::tests::run_test_data_config(nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("run_test_data_config validates config with entity collections",
          "[unit][harness]") {

  // Load sample test data which has entity collections
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(configs.value().size() >= 1);

  const auto *config = configs.value()[0];

  auto result = steamrot::tests::run_test_data_config(config);

  // Should succeed - validation passes
  REQUIRE(result.has_value());
}

TEST_CASE("run_test_data_config handles all sample data", "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Run wrapper on all sample configs
  for (const auto *config : configs.value()) {
    auto result = steamrot::tests::run_test_data_config(config);

    INFO("Test name: " << config->metadata()->test_name()->str());
    REQUIRE(result.has_value());
  }
}

TEST_CASE("run_test_data_config works with Catch2 generators",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  const auto *config = GENERATE_COPY(from_range(configs.value()));

  auto result = steamrot::tests::run_test_data_config(config);

  INFO("Test name: " << config->metadata()->test_name()->str());
  REQUIRE(result.has_value());
}

TEST_CASE("setup_fixture_from_test_data creates initialized fixture",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(configs.value().size() >= 1);

  const auto *config = configs.value()[0];

  // Setup fixture from test data
  auto fixture_result = steamrot::tests::setup_fixture_from_test_data(config);

  REQUIRE(fixture_result.has_value());

  auto &fixture = fixture_result.value();

  // Verify fixture is initialized
  REQUIRE(&fixture.GetGameResources() != nullptr);
  REQUIRE(&fixture.GetSceneResources() != nullptr);
  REQUIRE(&fixture.GetEntityManager() != nullptr);
}

TEST_CASE("setup_fixture_from_test_data rejects null config",
          "[unit][harness]") {

  auto result = steamrot::tests::setup_fixture_from_test_data(nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("run_single_test_with_fixture executes simulation function",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(configs.value().size() >= 1);

  const auto *config = configs.value()[0];

  bool simulation_called = false;

  // Run test with a simple simulation function
  auto result = steamrot::tests::run_single_test_with_fixture(
      config,
      [&simulation_called](steamrot::tests::TestFixture &fixture) {
        // Mark that the simulation was called
        simulation_called = true;

        // Verify we can access fixture resources
        REQUIRE(&fixture.GetEntityManager() != nullptr);
      });

  REQUIRE(result.has_value());
  REQUIRE(simulation_called);
}

TEST_CASE("run_test_with_fixture processes all test configs",
          "[unit][harness]") {

  size_t simulation_count = 0;

  // Run tests with a counting simulation function
  auto result = steamrot::tests::run_test_with_fixture(
      [&simulation_count](steamrot::tests::TestFixture &fixture) {
        simulation_count++;
        
        // Verify we can access fixture resources
        REQUIRE(&fixture.GetEntityManager() != nullptr);
      });

  REQUIRE(result.has_value());

  // Should have run simulation for each test config
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(simulation_count == configs.value().size());
}

TEST_CASE("run_single_test_with_fixture compares with expected state",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(configs.value().size() >= 1);

  // Use a config that has both start and expected entity collections
  const auto *config = configs.value()[0];
  REQUIRE(config->start_entity_collection() != nullptr);
  REQUIRE(config->expected_entity_collection() != nullptr);

  // Run test without modifying entities - should match expected state
  auto result = steamrot::tests::run_single_test_with_fixture(
      config,
      [](steamrot::tests::TestFixture &fixture) {
        // No modifications - entities should match expected state
      });

  REQUIRE(result.has_value());
}

