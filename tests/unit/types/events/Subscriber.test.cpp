/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the Subscriber class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Subscriber.h"
#include "EventPayload.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

TEST_CASE("Subscriber: check public members", "[unit][types][Subscriber]") {

  // create a Subscriber object
  steamrot::Subscriber subscriber{};
  subscriber.m_trigger_event_type = steamrot::EventType::TEST;

  // check the trigger event type
  REQUIRE(subscriber.m_trigger_event_type == steamrot::EventType::TEST);
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

TEST_CASE("Subscriber: with trigger payload", "[unit][types][Subscriber]") {

  steamrot::InputPayload input_payload{steamrot::InputPayload::InputAction::SELECT};
  steamrot::EventPayload trigger_payload = input_payload;

  // create a Subscriber object with trigger payload
  steamrot::Subscriber subscriber{};
  subscriber.m_trigger_event_type = steamrot::EventType::TEST;
  subscriber.m_trigger_event_data = trigger_payload;

  // check the trigger event type
  REQUIRE(subscriber.m_trigger_event_type == steamrot::EventType::TEST);
  // check the trigger event payload is set
  REQUIRE(subscriber.m_trigger_event_data.has_value());
  REQUIRE(std::holds_alternative<steamrot::InputPayload>(
      subscriber.m_trigger_event_data.value()));
}
