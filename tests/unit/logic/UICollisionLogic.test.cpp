/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UICollisionLogic class using data-driven approach
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_harness.h"
#include "UICollisionLogic.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("UICollisionLogic constructor creates instance without errors",
          "[unit][UICollisionLogic]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_fixture;

  REQUIRE_NOTHROW(
      steamrot::UICollisionLogic(test_fixture.GetSceneContext()));
}

TEST_CASE("UICollisionLogic data-driven tests", "[unit][UICollisionLogic][data-driven]") {
  // Load all test data configs from adjacent data/ directory
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Filter configs to only those for UICollisionLogic
  std::vector<const steamrot::TestDataConfig *> collision_configs;
  for (const auto *config : configs.value()) {
    if (config->metadata() && config->metadata()->test_name()) {
      std::string test_name = config->metadata()->test_name()->str();
      if (test_name.find("ui_collision") != std::string::npos) {
        collision_configs.push_back(config);
      }
    }
  }

  REQUIRE(collision_configs.size() > 0);

  // Use Catch2 generator to iterate through collision test configs
  const auto *config = GENERATE_COPY(from_range(collision_configs));

  INFO("Running test: " << config->metadata()->test_name()->str());
  if (config->metadata()->description()) {
    INFO("Description: " << config->metadata()->description()->str());
  }

  // Run the test using the test harness
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
