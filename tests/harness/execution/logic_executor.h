/////////////////////////////////////////////////
/// @file
/// @brief Level 1 executor for Logic class and free function execution
///
/// This executor provides the lowest level of test execution, allowing
/// individual Logic classes or free functions to be executed in isolation.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "Logic.h"
#include "SceneContext.h"
#include "simulation_generated.h"
#include <expected>

// Forward declaration to avoid circular dependency
namespace steamrot::tests {
class TestFixture;
}

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
/// @brief Execute a single Logic class instance
///
/// Wraps logic.RunLogic() with error handling.
///
/// @param logic Reference to the Logic instance to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ExecuteLogic(Logic &logic);

/////////////////////////////////////////////////
/// @brief Execute a Logic class by type enum
///
/// Creates an instance of the specified Logic class type and executes it.
///
/// @param type The LogicClassType enum specifying which Logic to create
/// @param scene_context Reference to the SceneContext for execution
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteLogicByType(LogicClassType type, SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Execute a free function by type enum
///
/// Dispatches to the appropriate free function based on the FunctionType.
///
/// @param type The FunctionType enum specifying which function to call
/// @param scene_context Reference to the SceneContext for execution
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteFunction(FunctionType type, SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Execute a single simulation step
///
/// Dispatches to either ExecuteLogicByType or ExecuteFunction based
/// on the step's execution mode.
///
/// @param step Pointer to the SimulationStep to execute
/// @param scene_context Reference to the SceneContext for execution
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSimulationStep(const SimulationStep *step, SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Execute a custom workflow (sequence of simulation steps)
///
/// Iterates through SimulationData steps and executes each in order.
///
/// @param simulation_data Pointer to the SimulationData containing steps
/// @param scene_context Reference to the SceneContext for execution
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteWorkflow(const SimulationData *simulation_data,
                SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Execute a workflow using a TestFixture
///
/// Convenience function that extracts the SceneContext from the fixture.
///
/// @param simulation_data Pointer to the SimulationData containing steps
/// @param fixture Reference to the TestFixture
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteWorkflowWithFixture(const SimulationData *simulation_data,
                           TestFixture &fixture);

} // namespace steamrot::tests::execution
