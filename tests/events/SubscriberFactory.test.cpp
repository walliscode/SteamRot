/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SubscriberFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberFactory.h"
#include "EventHandler.h"
#include <SFML/Window/Keyboard.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SubscriberFactory::CreateAndRegisterSubscriber",
          "[SubscriberFactory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // create a SubscriberFactory with the mock EventHandler
  steamrot::SubscriberFactory factory(mock_event_handler);

  // set up variables for the test
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;

  // check that the EventHandler UserInput register is empty initially
  auto &subscriber_register = mock_event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());

  // create and register a Subscriber
  auto create_result = factory.CreateAndRegisterSubscriber(event_type);
  if (!create_result.has_value())
    FAIL(create_result.error().message);

  // check that the Subscriber was created successfully
  REQUIRE(!subscriber_register.empty());
  REQUIRE(subscriber_register.size() == 1);
  REQUIRE(subscriber_register.at(event_type).size() == 1);
  REQUIRE(create_result.value() ==
          subscriber_register.at(event_type)[0].lock());
}

TEST_CASE("SubscriberFactory::CreateAndRegisterSubscriber with trigger event",
          "[SubscriberFactory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // create a SubscriberFactory with the mock EventHandler
  steamrot::SubscriberFactory factory(mock_event_handler);

  // set up variables for the test
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_TOGGLE_DROPDOWN;
  const steamrot::EventData trigger_data = std::string("specific_ui_element");

  // check that the EventHandler register is empty initially
  auto &subscriber_register = mock_event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());

  // create and register a Subscriber with trigger event
  auto create_result = factory.CreateAndRegisterSubscriber(event_type, trigger_data);
  if (!create_result.has_value())
    FAIL(create_result.error().message);

  // check that the Subscriber was created successfully
  REQUIRE(!subscriber_register.empty());
  REQUIRE(subscriber_register.size() == 1);
  REQUIRE(subscriber_register.at(event_type).size() == 1);
  REQUIRE(create_result.value() ==
          subscriber_register.at(event_type)[0].lock());
  
  // check that the trigger event is set correctly
  auto subscriber = create_result.value();
  auto trigger_event = subscriber->GetTriggerEvent();
  REQUIRE(trigger_event.has_value());
  REQUIRE(trigger_event.value() == trigger_data);
}
