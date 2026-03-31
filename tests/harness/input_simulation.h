/////////////////////////////////////////////////
/// @file
/// @brief Declaration of functions to simulate input events for testing
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
#include "InputEvent.h"
#include <SFML/System/Vector2.hpp>
#include <expected>
#include <unordered_map>
#include <vector>

namespace steamrot::tests {

// Execute all input events scheduled for a specific tick.
// Updates mouse_position and adds EventPackets to the event handler.
std::expected<std::monostate, FailInfo>
execute_input_events_for_tick(const std::vector<InputEvent> &input_events,
                              EventHandler &event_handler,
                              sf::Vector2i &mouse_position);

// Execute all events for the given tick.
std::expected<std::monostate, FailInfo>
execute_input_sequence(const std::unordered_map<size_t, std::vector<InputEvent>>
                           &input_events_by_tick,
                       size_t current_tick, EventHandler &event_handler,
                       sf::Vector2i &mouse_position);

} // namespace steamrot::tests
