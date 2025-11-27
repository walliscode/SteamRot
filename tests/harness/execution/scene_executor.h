/////////////////////////////////////////////////
/// @file
/// @brief Level 2 executor for Scene tick execution
///
/// This executor provides scene-level test execution, allowing full
/// scene ticks (Action -> Movement -> Collision -> Render) to be executed.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "LogicFactory.h"
#include "SceneContext.h"
#include "entities_generated.h"
#include "simulation_generated.h"
#include <expected>

// Forward declaration to avoid circular dependency
namespace steamrot::tests {
class TestFixture;
}

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
/// @brief Execute a complete scene tick using a LogicCollection
///
/// Executes all logic types in the standard scene update order:
/// Action -> Movement -> Collision -> Render
///
/// @param logic_map The LogicCollection containing scene logics
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneTick(const LogicCollection &logic_map);

/////////////////////////////////////////////////
/// @brief Execute a scene tick using TestFixture's logic collection
///
/// Gets the LogicCollection from the fixture and executes a full tick.
///
/// @param fixture Reference to the TestFixture
/// @param logic_map The LogicCollection to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneTickWithFixture(TestFixture &fixture,
                            const LogicCollection &logic_map);

/////////////////////////////////////////////////
/// @brief Configuration for scene tick execution
/////////////////////////////////////////////////
struct SceneTickConfig {
  /// Pointer to starting entity configuration (optional)
  const EntityCollection *start_entities = nullptr;

  /// Pointer to simulation data for custom logic execution (optional)
  const SimulationData *simulation = nullptr;

  /// Number of ticks to execute (default: 1)
  uint32_t num_ticks = 1;

  /// LogicCollection to use for scene tick execution
  const LogicCollection *logic_map = nullptr;
};

/////////////////////////////////////////////////
/// @brief Execute configured scene ticks
///
/// Handles multi-tick execution with optional entity injection
/// and simulation step execution.
///
/// @param fixture Reference to the TestFixture
/// @param config Configuration specifying how to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteConfiguredSceneTicks(TestFixture &fixture, const SceneTickConfig &config);

/////////////////////////////////////////////////
/// @brief Execute scene ticks with event processing
///
/// Wraps ExecuteSceneTick with event tick start/end processing
/// to match game engine behavior.
///
/// @param fixture Reference to the TestFixture
/// @param logic_map The LogicCollection to execute
/// @param num_ticks Number of ticks to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneTicksWithEvents(TestFixture &fixture,
                            const LogicCollection &logic_map,
                            uint32_t num_ticks = 1);

} // namespace steamrot::tests::execution
