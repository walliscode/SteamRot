/////////////////////////////////////////////////
/// @file
/// @brief Implementation of unified execution runner
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "execution_runner.h"
#include "TestFixture.h"
#include "game_loop_executor.h"
#include "logic_executor.h"
#include "scene_executor.h"
#include "scene_manager_executor.h"

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
const char *ExecutionLevelToString(ExecutionLevel level) {
  switch (level) {
  case ExecutionLevel::Logic:
    return "Logic (Level 1)";
  case ExecutionLevel::Scene:
    return "Scene (Level 2)";
  case ExecutionLevel::SceneManager:
    return "SceneManager (Level 3)";
  case ExecutionLevel::GameLoop:
    return "GameLoop (Level 4)";
  default:
    return "Unknown";
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunTestAtLevel(const TestDataConfig *config, ExecutionLevel level,
               TestFixture &fixture) {

  if (!config) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TestDataConfig is null"));
  }

  switch (level) {
  case ExecutionLevel::Logic: {
    // Level 1: Execute simulation steps (Logic/Function execution)
    if (config->simulation_data()) {
      return ExecuteWorkflowWithFixture(config->simulation_data(), fixture);
    }
    // No simulation data - nothing to execute at Logic level
    return std::monostate{};
  }

  case ExecutionLevel::Scene: {
    // Level 2: Execute scene ticks
    // Determine number of ticks from config
    uint32_t num_ticks = config->num_ticks() > 0 ? config->num_ticks() : 1;

    // Create an empty LogicCollection for basic scene tick simulation
    // In real usage, the LogicCollection would be provided by the test
    LogicCollection empty_collection;

    // Execute scene ticks with events
    return ExecuteSceneTicksWithEvents(fixture, empty_collection, num_ticks);
  }

  case ExecutionLevel::SceneManager: {
    // Level 3: Execute SceneManager updates
    uint32_t num_updates = config->num_ticks() > 0 ? config->num_ticks() : 1;
    return ExecuteSceneManagerWithFixture(fixture, num_updates);
  }

  case ExecutionLevel::GameLoop: {
    // Level 4: Execute game loop iterations
    GameLoopConfig loop_config;
    loop_config.num_iterations = config->num_ticks() > 0 ? config->num_ticks() : 1;
    loop_config.headless = true;

    // Set up event/input sequences if available
    if (config->event_sequence()) {
      loop_config.events = config->event_sequence();
    }
    if (config->input_sequence()) {
      loop_config.inputs = config->input_sequence();
    }

    return ExecuteGameLoopWithFixture(fixture, loop_config);
  }

  default:
    return std::unexpected(
        FailInfo(FailMode::NonExistentEnumValue, "Unknown execution level"));
  }
}

/////////////////////////////////////////////////
ExecutionLevel DetectExecutionLevel(const TestDataConfig *config) {
  if (!config) {
    return ExecutionLevel::Logic; // Default to lowest level
  }

  // Check for simulation_data with steps → Level 1 (Logic)
  if (config->simulation_data() && config->simulation_data()->steps() &&
      config->simulation_data()->steps()->size() > 0) {
    return ExecutionLevel::Logic;
  }

  // Check for scene transitions (would need scene change events) → Level 3
  // For now, check if we have event sequences that might indicate scene changes
  if (config->event_sequence() && config->event_sequence()->events()) {
    // If there are events, we might need SceneManager level
    // This is a heuristic - could be refined based on event types
    return ExecutionLevel::SceneManager;
  }

  // Check for multi-tick execution → Level 2 (Scene)
  if (config->num_ticks() > 1) {
    return ExecutionLevel::Scene;
  }

  // Default to Level 1 for simple tests
  return ExecutionLevel::Logic;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunTestAutoLevel(const TestDataConfig *config, TestFixture &fixture) {
  ExecutionLevel level = DetectExecutionLevel(config);
  return RunTestAtLevel(config, level, fixture);
}

} // namespace steamrot::tests::execution
