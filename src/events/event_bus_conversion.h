/////////////////////////////////////////////////
/// @file
/// @brief Declaration of EventBus conversion utilities
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
#include "event_bus_data_generated.h"
#include <expected>

namespace steamrot::event {

/////////////////////////////////////////////////
/// @brief Convert EventBusData to EventBus
///
/// Converts a FlatBuffers EventBusData representation to an in-game
/// EventBus (std::vector<EventPacket>). Each EventPacketData in the
/// EventBusData is converted to an EventPacket.
///
/// @param event_bus_data FlatBuffers EventBusData to convert
/// @return EventBus on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<EventBus, FailInfo>
ConvertEventBusDataToEventBus(const EventBusData *event_bus_data);

/////////////////////////////////////////////////
/// @brief Configure EventHandler's global event bus from EventBusData
///
/// Populates the EventHandler's global event bus with events from
/// the provided EventBusData. This is useful for initializing the
/// event bus state at the start of a test.
///
/// Note: This function clears the global event bus before adding events.
/// It does NOT clear the waiting room event bus.
///
/// @param event_bus_data FlatBuffers EventBusData to convert
/// @param event_handler EventHandler to configure
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEventHandlerFromEventBusData(const EventBusData *event_bus_data,
                                      EventHandler &event_handler);

} // namespace steamrot::event
