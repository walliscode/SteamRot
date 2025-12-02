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
/// @brief Execute all events in a vector
///
/// Processes all events that are in the provided vector.
/// Events are processed in the order they appear in the vector.
///
/// @param events The vector of EventPacketData to process
/// @param event_handler EventHandler to add events to
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InjectEventsForTick(const flatbuffers::Vector<EventPacketData *> *events,
                    EventHandler &event_handler);

} // namespace steamrot::tests
