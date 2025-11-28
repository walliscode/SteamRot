/////////////////////////////////////////////////
/// @file
/// @brief Declaration of event handler tick functions
///
/// Provides consolidated event bus processing as free functions
/// that can be used by both GameEngine and TestEngine.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"

namespace steamrot::events {

/////////////////////////////////////////////////
/// @brief Process the complete event bus cycle
///
/// Consolidates all event handling operations into a single call.
/// This ensures consistent ordering across game and test execution:
/// 1. ProcessWaitingRoomEventBus - move waiting room events to global bus
/// 2. UpateSubscribersFromGlobalEventBus - notify subscribers
/// 3. TickGlobalEventBus - decrement lifetimes, remove dead events
///
/// @param handler Reference to the EventHandler to process
/////////////////////////////////////////////////
void ProcessEventBusCycle(EventHandler &handler);

/////////////////////////////////////////////////
/// @brief Process the waiting room and update subscribers
///
/// A subset of event handling that processes the waiting room
/// and updates subscribers, but doesn't tick the event bus.
/// Useful when tick needs to happen after logic execution.
///
/// Operations:
/// 1. ProcessWaitingRoomEventBus - move waiting room events to global bus
/// 2. UpateSubscribersFromGlobalEventBus - notify subscribers
///
/// @param handler Reference to the EventHandler to process
/////////////////////////////////////////////////
void ProcessWaitingRoomAndUpdateSubscribers(EventHandler &handler);

} // namespace steamrot::events
