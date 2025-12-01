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
#include "EventHandler.h"
#include "FailInfo.h"
#include "GameContext.h"
#include "input_test_data_generated.h"
#include <expected>
#include <variant>

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
InjectInput(const InputEvent *input_event, EventHandler &event_handler,
            GameContext &game_context);

/////////////////////////////////////////////////
/// @brief Execute all input events for a specific tick
///
/// Processes all input events that are scheduled for the given tick number.
/// Events with the same tick are processed in the order they appear in the
/// input sequence.
///
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> InjectInputsForTick(
    const flatbuffers::Vector<flatbuffers::Offset<InputEvent>> *input_events,
    EventHandler &event_handler, GameContext &game_context);

} // namespace steamrot::tests
