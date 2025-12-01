/////////////////////////////////////////////////
/// @file
/// @brief Declaration of event simulation utilities for data-driven testing
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
#include "event_packet_data_generated.h"
#include "flatbuffers/vector.h"
#include <expected>
#include <variant>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Execute a single event test data entry
///
/// Converts the event test data to an EventPacket and adds it to the
/// event handler's waiting room event bus.
///
/// @param event_data The event test data to execute
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InjectEvent(const EventPacketData *event_packet_data,
            EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Execute all events for a specific tick
///
/// Processes all events that are scheduled for the given tick number.
/// Events with the same tick are processed in the order they appear in the
/// event sequence.
///
/// @param event_sequence The event sequence containing all events
/// @param tick The tick number to process
/// @param fixture TestFixture containing the test environment
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InjectEventsForTick(const flatbuffers::Vector<EventPacketData *> *events,
                    EventHandler &event_handler);

} // namespace steamrot::tests
