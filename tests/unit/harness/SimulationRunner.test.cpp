/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SimulationRunner class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SimulationRunner.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SimulationRunner constructor initializes successfully",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.description = "Test simulation";

  // Act & Assert
  REQUIRE_NOTHROW(
      steamrot::tests::SimulationRunner(simulation_data, scene_context));
}

TEST_CASE("SimulationRunner::ExecuteSimulation succeeds with empty steps",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.description = "Empty simulation";
  // No steps added

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(result.has_value());
}

TEST_CASE("SimulationRunner::ExecuteSimulation executes single step",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.description = "Single step simulation";
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIStateLogic});

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(result.has_value());
}

TEST_CASE("SimulationRunner::ExecuteSimulation executes multiple steps",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.description = "Multi-step simulation";
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UICollisionLogic});
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIStateLogic});
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIRenderLogic});

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(result.has_value());
}

TEST_CASE("SimulationRunner::ExecuteSimulation fails with invalid "
          "LogicClassEnum",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.description = "Invalid step simulation";
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::None});

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NonExistentEnumValue);
  REQUIRE(result.error().message ==
          "Invalid LogicClassEnum value in simulation step");
}

TEST_CASE("SimulationRunner::ExecuteSimulation executes UIActionLogic",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIActionLogic});

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(result.has_value());
}

TEST_CASE("SimulationRunner::ExecuteSimulation executes UICollisionLogic",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UICollisionLogic});

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(result.has_value());
}

TEST_CASE("SimulationRunner::ExecuteSimulation executes UIRenderLogic",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIRenderLogic});

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(result.has_value());
}

TEST_CASE("SimulationRunner::ExecuteSimulation executes UIStateLogic",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIStateLogic});

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(result.has_value());
}

TEST_CASE("SimulationRunner::ExecuteSimulation stops on first error",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.description = "Simulation with error in middle";
  // Add valid step
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIStateLogic});
  // Add invalid step
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::None});
  // Add another valid step (should not execute)
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIRenderLogic});

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NonExistentEnumValue);
}

TEST_CASE("SimulationRunner::ExecuteSimulation executes all logic types in "
          "sequence",
          "[unit][SimulationRunner]") {
  // Arrange
  steamrot::tests::TestFixture fixture(steamrot::SceneType::CRAFTING);
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::SimulationData simulation_data;
  simulation_data.description = "All logic types";
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIActionLogic});
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UICollisionLogic});
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIStateLogic});
  simulation_data.steps.push_back(
      steamrot::SimulationStep{steamrot::LogicClassEnum::UIRenderLogic});

  steamrot::tests::SimulationRunner runner(simulation_data, scene_context);

  // Act
  auto result = runner.ExecuteSimulation();

  // Assert
  REQUIRE(result.has_value());
}
