/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for simulation runner
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "simulation_runner.h"
#include "TestFixture.h"
#include "test_data_harness.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("execute_simulation_step validates null step", "[unit][harness][simulation]") {
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();
  
  auto &scene_context = fixture.GetSceneContext();
  
  auto result = steamrot::tests::execute_simulation_step(nullptr, scene_context);
  
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_simulation validates null simulation data", "[unit][harness][simulation]") {
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();
  
  auto &scene_context = fixture.GetSceneContext();
  
  auto result = steamrot::tests::execute_simulation(nullptr, scene_context);
  
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("execute_simulation_with_fixture validates null simulation data", "[unit][harness][simulation]") {
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();
  
  auto result = steamrot::tests::execute_simulation_with_fixture(nullptr, fixture);
  
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("Simulation execution with test data configs", "[unit][harness][simulation]") {
  // Load test data that includes simulation_data
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Filter to only configs with simulation_data
  std::vector<const steamrot::TestDataConfig *> simulation_configs;
  for (const auto *config : configs.value()) {
    if (config->simulation_data()) {
      simulation_configs.push_back(config);
    }
  }
  
  // Should have at least our sample simulation test
  REQUIRE(simulation_configs.size() >= 2);
  
  // Test each simulation config
  for (const auto *config : simulation_configs) {
    INFO("Testing simulation: " << config->metadata()->test_name()->str());
    
    // Create fixture from test data
    auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);
    REQUIRE(fixture_result.has_value());
    
    auto &fixture = fixture_result.value();
    
    // Execute simulation
    auto sim_result = steamrot::tests::execute_simulation_with_fixture(
        config->simulation_data(), fixture);
    
    // Simulation should complete successfully
    REQUIRE(sim_result.has_value());
  }
}

TEST_CASE("run_fixture_test executes simulations when present", "[unit][harness][simulation]") {
  // Load all test data
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Find a config with simulation_data
  const steamrot::TestDataConfig *sim_config = nullptr;
  for (const auto *config : configs.value()) {
    if (config->simulation_data()) {
      sim_config = config;
      break;
    }
  }
  
  REQUIRE(sim_config != nullptr);
  
  // run_fixture_test should execute the simulation
  auto result = steamrot::tests::run_fixture_test(sim_config);
  
  INFO("Test name: " << sim_config->metadata()->test_name()->str());
  REQUIRE(result.has_value());
}

TEST_CASE("Simulation with Catch2 generators", "[unit][harness][simulation]") {
  // Load all test data
  auto configs_result = steamrot::tests::load_test_data_configs();
  REQUIRE(configs_result.has_value());
  
  // Filter to configs with simulation data
  std::vector<const steamrot::TestDataConfig *> sim_configs;
  for (const auto *config : configs_result.value()) {
    if (config->simulation_data()) {
      sim_configs.push_back(config);
    }
  }
  
  REQUIRE(sim_configs.size() >= 1);
  
  // Use generator
  const auto *config = GENERATE_COPY(from_range(sim_configs));
  
  INFO("Running simulation test: " << config->metadata()->test_name()->str());
  
  // Execute via run_fixture_test
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}

TEST_CASE("Simulation execution modes work correctly", "[unit][harness][simulation]") {
  steamrot::tests::TestFixture fixture;
  fixture.Intialize();
  
  auto &scene_context = fixture.GetSceneContext();
  
  SECTION("Function execution mode") {
    // Create a simple simulation step with function execution
    flatbuffers::FlatBufferBuilder builder;
    
    auto step_offset = steamrot::CreateSimulationStep(
        builder,
        steamrot::SimulationType::Action,
        steamrot::ExecutionMode::Function,
        steamrot::FunctionType::ProcessNestedUIActionsAndEvents);
    
    builder.Finish(step_offset);
    
    const steamrot::SimulationStep *step =
        flatbuffers::GetRoot<steamrot::SimulationStep>(builder.GetBufferPointer());
    
    auto result = steamrot::tests::execute_simulation_step(step, scene_context);
    REQUIRE(result.has_value());
  }
  
  SECTION("LogicClass execution mode") {
    // Create a simple simulation step with LogicClass execution
    flatbuffers::FlatBufferBuilder builder;
    
    auto step_offset = steamrot::CreateSimulationStep(
        builder,
        steamrot::SimulationType::Action,
        steamrot::ExecutionMode::LogicClass,
        steamrot::FunctionType::None,
        steamrot::LogicClassType::UIActionLogic);
    
    builder.Finish(step_offset);
    
    const steamrot::SimulationStep *step =
        flatbuffers::GetRoot<steamrot::SimulationStep>(builder.GetBufferPointer());
    
    auto result = steamrot::tests::execute_simulation_step(step, scene_context);
    REQUIRE(result.has_value());
  }
}
