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
