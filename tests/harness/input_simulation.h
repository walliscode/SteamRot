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
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <expected>
#include <unordered_map>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Inject pre-built sf::Events into the EventHandler for a single tick.
///
/// Passes the events through EventHandler::ConvertSFMLEventsToEventPackets so
/// that the existing bitset accumulation, action-resolution, and EventPacket
/// creation pipeline runs exactly as it would with real hardware events.
///
/// @param sfml_events Events to inject for this tick.
/// @param event_handler EventHandler to receive the converted EventPackets.
/// @return std::monostate on success, FailInfo on error.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_events_for_tick(const std::vector<sf::Event> &sfml_events,
                              EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Inject input events for the current tick from a per-tick map.
///
/// Looks up @p current_tick in @p input_events_by_tick. If a matching entry
/// exists, delegates to execute_input_events_for_tick. Returns immediately
/// (success) when no events are scheduled for this tick.
///
/// @param input_events_by_tick Map of tick -> events to inject.
/// @param current_tick Tick number to process.
/// @param event_handler EventHandler to receive the converted EventPackets.
/// @return std::monostate on success, FailInfo on error.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_input_sequence(
    const std::unordered_map<size_t, std::vector<sf::Event>>
        &input_events_by_tick,
    size_t current_tick, EventHandler &event_handler);

} // namespace steamrot::tests
