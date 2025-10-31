/////////////////////////////////////////////////
/// @file
/// @brief Declaration of input simulation utilities for data-driven testing
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
#include "input_test_data_generated.h"
#include <expected>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Execute a single input event
///
/// Injects a single input event into the test fixture. The input event
/// is converted to the appropriate SFML event type and applied to the
/// context (e.g., updating mouse position, triggering keyboard events).
///
/// @param input_event The input event to execute
/// @param fixture TestFixture containing the test environment
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_event(const InputEvent *input_event, TestFixture &fixture);

/////////////////////////////////////////////////
/// @brief Execute all input events for a specific tick
///
/// Processes all input events that are scheduled for the given tick number.
/// Events with the same tick are processed in the order they appear in the
/// input sequence.
///
/// @param input_sequence The input sequence containing all events
/// @param tick The tick number to process
/// @param fixture TestFixture containing the test environment
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_events_for_tick(const InputSequence *input_sequence, uint32_t tick,
                               TestFixture &fixture);

/////////////////////////////////////////////////
/// @brief Execute a complete input sequence
///
/// Processes all input events in the sequence, calling them at the appropriate
/// tick. This is a convenience function that iterates through all ticks and
/// calls execute_input_events_for_tick for each unique tick value.
///
/// Note: This function does NOT advance simulation ticks automatically.
/// It only injects the inputs. The caller is responsible for running
/// simulations or logic between ticks as needed.
///
/// @param input_sequence The input sequence to execute
/// @param fixture TestFixture containing the test environment
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_sequence(const InputSequence *input_sequence,
                       TestFixture &fixture);

} // namespace steamrot::tests
