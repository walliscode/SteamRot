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
} // namespace steamrot::events
