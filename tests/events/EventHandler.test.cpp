/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the EventHandler class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "Subscriber.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("EventHandler registers Subscribers", "[EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;

  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;
  const steamrot::UserInputBitset user_input_bitset{2};

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type, user_input_bitset);

  // Check that the EventHandler UserInput register is empty initially
  auto user_input_register = event_handler.GetUserInputRegister();
  REQUIRE(user_input_register.empty());

  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    FAIL(result.error().message);

  // Check that the Subscriber was registered successfully
  user_input_register = event_handler.GetUserInputRegister();

  REQUIRE(user_input_register.size() == 1);
  REQUIRE(user_input_register.count(user_input_bitset) == 1);
  // check that the pointers are the same
  REQUIRE(subscriber == user_input_register[user_input_bitset][0].lock());

  // when the subscriber goes out of scope, the weak pointer should be expired
  subscriber.reset();
  REQUIRE(user_input_register[user_input_bitset][0].expired());
}
