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
/// Creates a UserInputBitset directly from the input event data and generates
/// an EVENT_USER_INPUT EventPacket that is added to the EventHandler.
/// Mouse position is also updated in GameContext for mouse events.
/// MouseMove events only update position without generating EventPackets.
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
/// Processes all input events in the sequence, generating EventPackets at the
/// appropriate tick. This is a convenience function that iterates through all
/// ticks and calls execute_input_events_for_tick for each unique tick value.
///
/// Note: This function does NOT advance simulation ticks automatically or
/// process the waiting room event bus. It only adds EventPackets to the
/// waiting room. The caller is responsible for calling
/// ProcessWaitingRoomEventBus() and running simulations between ticks.
///
/// @param input_sequence The input sequence to execute
/// @param fixture TestFixture containing the test environment
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_sequence(const InputSequence *input_sequence,
                       TestFixture &fixture);

} // namespace steamrot::tests
