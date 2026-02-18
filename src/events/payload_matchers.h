/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for matching payloads of events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "EventPacket.h"
#include "EventPayload.h"
namespace steamrot::events {

/////////////////////////////////////////////////
/// @brief Returns true if the filter payload matches the event payload.
///
/// This matching does not need to be a 1-2-1 match and can be dictated by more
/// complex logic
///
/// @param filter_payload Payload living on the EventPacket from static objects
/// such as Subscribers.
/// @param event_payload  Payload living on the EventPacket from the EventBus
/// @return True if they match
/////////////////////////////////////////////////
bool MatchPayload(const InputPayload &filter_payload,
                  const InputPayload &event_payload);

/////////////////////////////////////////////////
/// @brief Returns true if the filter payload matches the event payload.
///
/// @param filter_payload UIPayload from Subscriber filter
/// @param event_payload  UIPayload from EventBus
/// @return True if they match
/////////////////////////////////////////////////
bool MatchPayload(const UIPayload &filter_payload,
                  const UIPayload &event_payload);

/////////////////////////////////////////////////
/// @brief Returns true if the filter payload matches the event payload.
///
/// @param filter_payload LogicPayload from Subscriber filter
/// @param event_payload  LogicPayload from EventBus
/// @return True if they match
/////////////////////////////////////////////////
bool MatchPayload(const LogicPayload &filter_payload,
                  const LogicPayload &event_payload);

/////////////////////////////////////////////////
/// @brief Returns true if the filter payload matches the event payload.
///
/// @param filter_payload ScenePayload from Subscriber filter
/// @param event_payload  ScenePayload from EventBus
/// @return True if they match
/////////////////////////////////////////////////
bool MatchPayload(const ScenePayload &filter_payload,
                  const ScenePayload &event_payload);

/////////////////////////////////////////////////
/// @brief Returns true if the filter payload matches the event payload.
///
/// @param filter_payload SystemPayload from Subscriber filter
/// @param event_payload  SystemPayload from EventBus
/// @return True if they match
/////////////////////////////////////////////////
bool MatchPayload(const SystemPayload &filter_payload,
                  const SystemPayload &event_payload);

/////////////////////////////////////////////////
/// @brief Returns true if the filter EventPacket matches the event EventPacket.
///
/// Returns false if the variant types do not match or if the payloads do not
/// match according to their specific matching logic.
///
/// @param filter_packet EventPacket from Subscriber filter
/// @param event_packet  EventPacket from EventBus
/// @return True if they match
/////////////////////////////////////////////////
bool MatchEventPacket(const EventPacket &filter_packet,
                      const EventPacket &event_packet);

} // namespace steamrot::events
