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
#include "FailInfo.h"
#include "TestFixture.h"
#include "event_test_data_generated.h"
#include <expected>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Execute a single event test data entry
///
/// Converts the event test data to an EventPacket and adds it to the
/// event handler's waiting room event bus.
///
/// @param event_data The event test data to execute
/// @param fixture TestFixture containing the test environment
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteEventTestData(const EventTestData *event_data, TestFixture &fixture);

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
ExecuteEventsForTick(const EventSequence *event_sequence, uint32_t tick,
                     TestFixture &fixture);

/////////////////////////////////////////////////
/// @brief Execute a complete event sequence
///
/// Processes all events in the sequence, adding them at the appropriate
/// tick. This is a convenience function that iterates through all ticks and
/// calls ExecuteEventsForTick for each unique tick value.
///
/// Note: This function does NOT process the waiting room or tick the event bus.
/// It only adds events to the waiting room. The caller is responsible for
/// calling ProcessWaitingRoomEventBus() and TickGlobalEventBus() as needed.
///
/// @param event_sequence The event sequence to execute
/// @param fixture TestFixture containing the test environment
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteEventSequence(const EventSequence *event_sequence,
                     TestFixture &fixture);

} // namespace steamrot::tests
