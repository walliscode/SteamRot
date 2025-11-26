/////////////////////////////////////////////////
/// @file
/// @brief Implementation of event processing free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_processing.h"

namespace steamrot::events::processing {

/////////////////////////////////////////////////
void ProcessEventTickStart(EventHandler &event_handler,
                           sf::RenderWindow *window) {
  // 1. Preload external events (SFML events) if window is provided
  if (window) {
    event_handler.PreloadEvents(*window);
  }

  // 2. Move events from waiting room to global event bus
  event_handler.ProcessWaitingRoomEventBus();

  // 3. Update subscribers based on events in global bus
  event_handler.UpateSubscribersFromGlobalEventBus();
}

/////////////////////////////////////////////////
void ProcessEventTickEnd(EventHandler &event_handler) {
  // Tick the global event bus (decrement lifetimes, remove expired)
  event_handler.TickGlobalEventBus();
}

/////////////////////////////////////////////////
void ProcessFullEventTick(EventHandler &event_handler,
                          sf::RenderWindow *window) {
  ProcessEventTickStart(event_handler, window);
  ProcessEventTickEnd(event_handler);
}

} // namespace steamrot::events::processing
