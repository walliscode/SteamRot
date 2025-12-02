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
/// @param event_handler EventHandler to add events to
/// @param game_context GameContext for updating mouse position
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InjectInput(const InputEvent *input_event, EventHandler &event_handler,
            GameContext &game_context);

/////////////////////////////////////////////////
/// @brief Execute all input events in a vector
///
/// Processes all input events that are in the provided vector.
/// Events are processed in the order they appear in the vector.
///
/// @param input_events The vector of InputEvent to process
/// @param event_handler EventHandler to add events to
/// @param game_context GameContext for updating mouse position
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> InjectInputsForTick(
    const flatbuffers::Vector<flatbuffers::Offset<InputEvent>> *input_events,
    EventHandler &event_handler, GameContext &game_context);

} // namespace steamrot::tests
