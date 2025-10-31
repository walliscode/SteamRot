/////////////////////////////////////////////////
/// @file
/// @brief Declaration of tick-based test execution utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "TestFixture.h"
#include "test_data_generated.h"
#include <expected>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Execute test for a single tick
///
/// Executes all inputs, events, and simulation steps scheduled for the
/// specified tick, then performs per-tick maintenance (event bus ticking).
///
/// @param tick The tick number to execute
/// @param config Test data configuration containing inputs/events/simulation
/// @param fixture TestFixture containing the test environment
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_single_tick(uint32_t tick, const TestDataConfig *config,
                    TestFixture &fixture);

/////////////////////////////////////////////////
/// @brief Determine the number of ticks to execute for a test
///
/// If config->num_ticks() is specified, returns that value.
/// Otherwise, auto-detects the number of ticks by finding the maximum
/// tick value in input_sequence, event_sequence, and simulation_data,
/// then adds 1 (since ticks are 0-based).
///
/// @param config Test data configuration
/// @return Number of ticks to execute (minimum 1)
/////////////////////////////////////////////////
uint32_t determine_num_ticks(const TestDataConfig *config);

/////////////////////////////////////////////////
/// @brief Execute a complete tick-based test
///
/// Runs the test for the specified number of ticks (or auto-detected),
/// executing inputs, events, and simulation steps at each tick.
///
/// Execution order per tick:
/// 1. Execute inputs scheduled for this tick
/// 2. Execute events scheduled for this tick
/// 3. Process event waiting room
/// 4. Execute simulation steps scheduled for this tick
/// 5. Tick the global event bus
///
/// @param config Test data configuration
/// @param fixture TestFixture containing the test environment
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_tick_based_test(const TestDataConfig *config, TestFixture &fixture);

} // namespace steamrot::tests
