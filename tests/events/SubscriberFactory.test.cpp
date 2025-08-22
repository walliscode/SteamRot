/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SubscriberFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberFactory.h"
#include "EventHandler.h"
#include "event_helpers.h"
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
  const steamrot::UserInputBitset user_input_bitset{2};

  // check that the EventHandler UserInput register is empty initially
  auto &user_input_register = mock_event_handler.GetUserInputRegister();
  REQUIRE(user_input_register.empty());

  // create and register a Subscriber
  auto create_result =
      factory.CreateAndRegisterSubscriber(event_type, user_input_bitset);
  if (!create_result.has_value())
    FAIL(create_result.error().message);

  // check that the Subscriber was created successfully
  REQUIRE(!user_input_register.empty());
  REQUIRE(user_input_register.size() == 1);
  REQUIRE(user_input_register.count(user_input_bitset) == 1);
  REQUIRE(create_result.value() ==
          user_input_register.find(user_input_bitset)->second[0].lock());
}
