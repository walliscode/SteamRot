/////////////////////////////////////////////////
/// @file
/// @brief Declaration of simulation runner for data-driven logic testing
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneContext.h"
#include "simulation_generated.h"
#include <expected>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Execute a single simulation step
///
/// Dispatches to the appropriate function or Logic class based on the
/// step configuration. This is the core dispatcher that uses switch/case
/// to call individual free functions or instantiate and run Logic classes.
///
/// @param step The simulation step to execute
/// @param scene_context SceneContext containing scene resources and entities
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSimulationStep(const SimulationStep *step, SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Execute a complete simulation sequence
///
/// Runs all steps in the simulation data in order. Each step can be
/// either a function call or a Logic class execution.
///
/// @param simulation_data The simulation configuration with steps
/// @param scene_context SceneContext containing scene resources and entities
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSimulation(const SimulationData *simulation_data,
                  SceneContext &scene_context);

} // namespace steamrot::tests
