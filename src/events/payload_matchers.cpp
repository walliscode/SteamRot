/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for matching payloads of events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "payload_matchers.h"

namespace steamrot::events {
/////////////////////////////////////////////////
bool MatchPayload(const InputPayload &filter_payload,
                  const InputPayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }
  return true;
}
} // namespace steamrot::events
