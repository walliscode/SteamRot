/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIActionLogic class using data-driven approach
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_harness.h"
#include "UIActionLogic.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("UIActionLogic constructor creates instance without errors",
          "[unit][UIActionLogic]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_fixture;

  REQUIRE_NOTHROW(
      steamrot::UIActionLogic(test_fixture.GetSceneContext()));
}

TEST_CASE("UIActionLogic data-driven tests", "[unit][UIActionLogic][data-driven]") {
  // Load all test data configs from adjacent data/ directory
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Filter configs to only those for UIActionLogic
  std::vector<const steamrot::TestDataConfig *> action_configs;
  for (const auto *config : configs.value()) {
    if (config->metadata() && config->metadata()->test_name()) {
      std::string test_name = config->metadata()->test_name()->str();
      if (test_name.find("ui_action") != std::string::npos) {
        action_configs.push_back(config);
      }
    }
  }

  REQUIRE(action_configs.size() > 0);

  // Use Catch2 generator to iterate through action test configs
  const auto *config = GENERATE_COPY(from_range(action_configs));

  INFO("Running test: " << config->metadata()->test_name()->str());
  if (config->metadata()->description()) {
    INFO("Description: " << config->metadata()->description()->str());
  }

  // Run the test using the test harness
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
