/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the Subscriber class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Subscriber.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

TEST_CASE("Subscriber: check public members", "[unit][Subscriber]") {

  // create a Subscriber object
  steamrot::Subscriber subscriber{steamrot::EventType::EventType_EVENT_TEST};

  // check the trigger event type
  REQUIRE(subscriber.m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_TEST);
  // check the trigger event data is empty
  REQUIRE(!subscriber.m_trigger_event_data.has_value());

  // check the subscriber is not active
  REQUIRE(!subscriber.m_active);
  // activate the subscriber
  subscriber.m_active = true;
  REQUIRE(subscriber.m_active);
  // deactivate the subscriber
  subscriber.m_active = false;
  REQUIRE(!subscriber.m_active);
}

TEST_CASE("Subscriber: with trigger data", "[unit][Subscriber]") {

  steamrot::EventData trigger_data = steamrot::UserInputBitset{};

  // create a Subscriber object with trigger data
  steamrot::Subscriber subscriber{steamrot::EventType::EventType_EVENT_TEST,
                                  trigger_data};

  // check the trigger event type
  REQUIRE(subscriber.m_trigger_event_type ==
          steamrot::EventType::EventType_EVENT_TEST);
  // check the trigger event data is set
  REQUIRE(subscriber.m_trigger_event_data.has_value());
  REQUIRE(std::holds_alternative<steamrot::UserInputBitset>(
      subscriber.m_trigger_event_data.value()));
}
