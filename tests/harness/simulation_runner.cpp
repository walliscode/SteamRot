/////////////////////////////////////////////////
/// @file
/// @brief Implementation of simulation runner for data-driven logic testing
///
/// This module now delegates to the execution module's logic_executor
/// for Level 1 execution, maintaining backward compatibility.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "simulation_runner.h"
#include "logic_executor.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSimulationStep(const SimulationStep *step, SceneContext &scene_context) {
  // Delegate to the execution module's implementation
  return execution::ExecuteSimulationStep(step, scene_context);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSimulation(const SimulationData *simulation_data,
                  SceneContext &scene_context) {
  // Delegate to the execution module's implementation
  return execution::ExecuteWorkflow(simulation_data, scene_context);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSimulationWithFixture(const SimulationData *simulation_data,
                             TestFixture &fixture) {
  // Delegate to the execution module's implementation
  return execution::ExecuteWorkflowWithFixture(simulation_data, fixture);
}

} // namespace steamrot::tests
