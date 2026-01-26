/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SimulationRunner class
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
#include "SimulationData.h"
#include <expected>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class SimulationRunner
/// @brief Executes simulation steps for TestEngine.
///
/// The SimulationRunner orchestrates execution of Logic class
/// instances as specified in SimulationData.
/// Each step is executed in order, with proper error handling.
/////////////////////////////////////////////////
class SimulationRunner {
private:
  /////////////////////////////////////////////////
  /// @brief Reference to simulation configuration
  /////////////////////////////////////////////////
  const SimulationData &m_simulation_data;

  /////////////////////////////////////////////////
  /// @brief Reference to scene context for execution
  /////////////////////////////////////////////////
  SceneContext &m_scene_context;

  /////////////////////////////////////////////////
  /// @brief Execute a single simulation step
  ///
  /// @param step The simulation step to execute
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteStep(const SimulationStep &step);

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param simulation_data Reference to simulation configuration
  /// @param scene_context Reference to scene context
  /////////////////////////////////////////////////
  SimulationRunner(const SimulationData &simulation_data,
                   SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute all simulation steps
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ExecuteSimulation();
};

} // namespace steamrot::tests
