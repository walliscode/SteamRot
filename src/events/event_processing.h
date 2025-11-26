/////////////////////////////////////////////////
/// @file
/// @brief Free functions for event processing.
///
/// These functions extract the core event processing patterns from GameEngine,
/// enabling reuse by both the game engine and test harness.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace steamrot::events::processing {

/////////////////////////////////////////////////
/// @brief Process events at the start of a tick
///
/// Performs the event handling sequence that occurs before logic execution:
/// 1. PreloadEvents (poll SFML events if window provided)
/// 2. ProcessWaitingRoomEventBus (move waiting room to global bus)
/// 3. UpdateSubscribersFromGlobalEventBus (notify subscribers)
///
/// @param event_handler Reference to the EventHandler to process
/// @param window Optional pointer to RenderWindow for SFML event polling.
///               Pass nullptr for headless/test mode.
/////////////////////////////////////////////////
void ProcessEventTickStart(EventHandler &event_handler,
                           sf::RenderWindow *window = nullptr);

/////////////////////////////////////////////////
/// @brief Complete an event tick (after logic execution)
///
/// Performs the event handling sequence that occurs after logic execution:
/// 1. TickGlobalEventBus (decrement lifetimes, remove expired events)
///
/// @param event_handler Reference to the EventHandler to process
/////////////////////////////////////////////////
void ProcessEventTickEnd(EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Execute a full event tick (start + end)
///
/// Convenience function that combines ProcessEventTickStart and
/// ProcessEventTickEnd. Note: In normal game flow, logic execution
/// occurs between these two phases.
///
/// @param event_handler Reference to the EventHandler to process
/// @param window Optional pointer to RenderWindow for SFML event polling.
/////////////////////////////////////////////////
void ProcessFullEventTick(EventHandler &event_handler,
                          sf::RenderWindow *window = nullptr);

} // namespace steamrot::events::processing
