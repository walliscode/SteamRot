/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for simulation runner
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "simulation_runner.h"
#include "EntityManager.h"
#include "GameCore.h"
#include "SceneContext.h"
#include "SceneCore.h"
#include "test_data_loader.h"
#include "test_harness.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("ExecuteSimulationStep validates null step",
          "[unit][harness][simulation]") {
  // Create minimal mock SceneContext components
  steamrot::EntityManager entity_manager;
  steamrot::GameCore game_core;
  steamrot::SceneCore scene_core;

  // Note: This test verifies null handling at the API level
  // The SceneContext won't be used if step is null
  steamrot::SceneContext scene_context(scene_core, game_core, entity_manager);

  auto result =
      steamrot::tests::ExecuteSimulationStep(nullptr, scene_context);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("ExecuteSimulation validates null simulation data",
          "[unit][harness][simulation]") {
  // Create minimal mock SceneContext components
  steamrot::EntityManager entity_manager;
  steamrot::GameCore game_core;
  steamrot::SceneCore scene_core;

  steamrot::SceneContext scene_context(scene_core, game_core, entity_manager);

  auto result = steamrot::tests::ExecuteSimulation(nullptr, scene_context);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("RunTestHarness with simulation test data configs",
          "[unit][harness][simulation]") {
  // Load test data that includes simulation_data
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Filter to only configs with simulation_data
  std::vector<steamrot::TestDataConfig *> simulation_configs;
  for (const auto *config : configs.value()) {
    if (config->simulation_data()) {
      simulation_configs.push_back(
          const_cast<steamrot::TestDataConfig *>(config));
    }
  }

  // Should have at least our sample simulation test
  REQUIRE(simulation_configs.size() >= 2);

  // Test each simulation config using TestEngine via RunTestHarness
  for (auto *config : simulation_configs) {
    INFO("Testing simulation: " << config->metadata()->test_name()->str());

    auto result = steamrot::tests::RunTestHarness(config);

    // Simulation should complete successfully
    REQUIRE(result.has_value());
  }
}

TEST_CASE("Simulation with Catch2 generators", "[unit][harness][simulation]") {
  // Load all test data
  auto configs_result = steamrot::tests::load_test_data_configs();
  REQUIRE(configs_result.has_value());

  // Filter to configs with simulation data
  std::vector<steamrot::TestDataConfig *> sim_configs;
  for (const auto *config : configs_result.value()) {
    if (config->simulation_data()) {
      sim_configs.push_back(const_cast<steamrot::TestDataConfig *>(config));
    }
  }

  REQUIRE(sim_configs.size() >= 1);

  // Use generator
  auto *config = GENERATE_COPY(from_range(sim_configs));

  INFO("Running simulation test: " << config->metadata()->test_name()->str());

  // Execute via RunTestHarness
  auto result = steamrot::tests::RunTestHarness(config);
  REQUIRE(result.has_value());
}
