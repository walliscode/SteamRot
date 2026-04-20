/////////////////////////////////////////////////
/// @file
/// @brief Declaration of SimulationData struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <string>
#include <vector>
namespace steamrot {

//////////////////////////////////////////////
/// @brief Represents a logic class to be simulated
///
/// This enum is used to map directly to classes use for logic
//////////////////////////////////////////////
enum class LogicClassEnum {
  None = 0,
  UIActionLogic,
  UICollisionLogic,
  UIRenderLogic,
  UIStateLogic,
  GrimoireMachinaActionLogic,
  GrimoireMachinaPositioningLogic,
  GrimoireMachinaCollisionLogic,
  GrimoireMachinaRenderLogic

};

using SimulationElement = LogicClassEnum;

/////////////////////////////////////////////////
/// @class SimulationStep
/// @brief All information required for a single simulation step
/////////////////////////////////////////////////
struct SimulationStep {

  /////////////////////////////////////////////////
  /// @brief Constructor taking in a simulation element
  ///
  /// @param element Simulation element to be simulated
  /////////////////////////////////////////////////
  SimulationStep(SimulationElement element) : element(element) {}

  /////////////////////////////////////////////////
  /// @brief A function or class to be simulated
  /////////////////////////////////////////////////
  SimulationElement element;
};

/////////////////////////////////////////////////
/// @class SimulationData
/// @brief Contains all information required to run a simulation
/////////////////////////////////////////////////
struct SimulationData {

  /////////////////////////////////////////////////
  /// @brief Description of the overall simulation
  /////////////////////////////////////////////////
  std::string description{};

  /////////////////////////////////////////////////
  /// @brief Ordered list of simulation steps to be executed
  /////////////////////////////////////////////////
  std::vector<SimulationStep> steps{};

  /////////////////////////////////////////////////
  /// @brief When true, the TestEngine uses the scene's pre-built logic_map
  /// (as configured by LogicFactory) instead of the explicit steps list.
  ///
  /// This exactly mirrors the production GameEngine behaviour: each scene runs
  /// sCollision, sAction, sPositioning and sRender in the standard order using
  /// the Logic objects that LogicFactory created for that scene type.
  /// The steps list is ignored when this flag is set.
  /////////////////////////////////////////////////
  bool use_default_logic{false};
};
} // namespace steamrot
