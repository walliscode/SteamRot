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
/// Returns the value of config->num_ticks() if specified and greater than 0.
/// Otherwise, returns 1 (single tick execution).
///
/// Note: This function ONLY uses the TestDataConfig level num_ticks field.
/// It does NOT auto-detect from input_sequence, event_sequence, or 
/// simulation_data()->num_ticks(). Those fields are not consulted.
///
/// @param config Test data configuration
/// @return Number of ticks to execute (minimum 1)
/////////////////////////////////////////////////
uint32_t determine_num_ticks(const TestDataConfig *config);

/////////////////////////////////////////////////
/// @brief Execute a complete tick-based test
///
/// Runs the test for the number of ticks specified by config->num_ticks(),
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
