/////////////////////////////////////////////////
/// @file
/// @brief Implementation of event handler tick functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_handler_tick.h"

namespace steamrot::events {

/////////////////////////////////////////////////
void ProcessEventBusCycle(EventHandler &handler) {
  handler.ProcessWaitingRoomEventBus();
  handler.UpateSubscribersFromGlobalEventBus();
  handler.TickGlobalEventBus();
}

/////////////////////////////////////////////////
void ProcessWaitingRoomAndUpdateSubscribers(EventHandler &handler) {
  handler.ProcessWaitingRoomEventBus();
  handler.UpateSubscribersFromGlobalEventBus();
}

} // namespace steamrot::events
