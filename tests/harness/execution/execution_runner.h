/////////////////////////////////////////////////
/// @file
/// @brief Unified execution runner for multi-level test execution
///
/// This module provides a unified entry point for running tests at
/// different execution levels, automatically detecting the appropriate
/// level from test configuration.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "test_data_generated.h"
#include <expected>

// Forward declaration to avoid circular dependency
namespace steamrot::tests {
class TestFixture;
}

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
/// @brief Execution level enum
///
/// Defines the four levels of test execution granularity:
/// - Logic: Single Logic class or free function
/// - Scene: Full scene tick (all logics in order)
/// - SceneManager: Full SceneManager update cycle
/// - GameLoop: Complete game loop iteration
/////////////////////////////////////////////////
enum class ExecutionLevel {
  Logic = 1,       ///< Level 1: Individual Logic/function execution
  Scene = 2,       ///< Level 2: Full scene tick
  SceneManager = 3, ///< Level 3: SceneManager update cycle
  GameLoop = 4     ///< Level 4: Complete game loop iteration
};

/////////////////////////////////////////////////
/// @brief Convert ExecutionLevel to string for logging
///
/// @param level The execution level
/// @return String representation of the level
/////////////////////////////////////////////////
const char *ExecutionLevelToString(ExecutionLevel level);

/////////////////////////////////////////////////
/// @brief Run test at specified execution level
///
/// Dispatches to the appropriate executor based on the specified level.
///
/// @param config Pointer to the TestDataConfig
/// @param level The execution level to use
/// @param fixture Reference to the TestFixture
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunTestAtLevel(const TestDataConfig *config, ExecutionLevel level,
               TestFixture &fixture);

/////////////////////////////////////////////////
/// @brief Auto-detect execution level from config
///
/// Analyzes the TestDataConfig to determine the most appropriate
/// execution level based on what data is present:
/// - If simulation_data has steps → Level 1 (Logic)
/// - If num_ticks > 0 but no simulation → Level 2 (Scene)
/// - If scene transitions detected → Level 3 (SceneManager)
/// - If full game loop needed → Level 4 (GameLoop)
///
/// @param config Pointer to the TestDataConfig
/// @return The detected execution level
/////////////////////////////////////////////////
ExecutionLevel DetectExecutionLevel(const TestDataConfig *config);

/////////////////////////////////////////////////
/// @brief Run test with auto-detected execution level
///
/// Convenience function that detects the appropriate level and runs.
///
/// @param config Pointer to the TestDataConfig
/// @param fixture Reference to the TestFixture
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunTestAutoLevel(const TestDataConfig *config, TestFixture &fixture);

} // namespace steamrot::tests::execution
