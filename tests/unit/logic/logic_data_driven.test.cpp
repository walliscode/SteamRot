/////////////////////////////////////////////////
/// @file
/// @brief Data-driven tests for Logic classes using test harness
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_harness.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Logic classes data-driven tests", "[unit][logic][data-driven]") {
  // Load all test data configs from adjacent data/ directory
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(configs.value().size() > 0);

  // Use Catch2 generator to iterate through all test configs
  const auto *config = GENERATE_COPY(from_range(configs.value()));

  INFO("Running test: " << config->metadata()->test_name()->str());
  if (config->metadata()->description()) {
    INFO("Description: " << config->metadata()->description()->str());
  }

  // Run the test using the test harness
  auto result = steamrot::tests::RunFixtureTest(config);
  if (!result.has_value()) {
    FAIL("Test execution failed: " << result.error().message);
  }
}
