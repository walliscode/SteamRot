/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SubscriberFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberFactory.h"
#include "EventHandler.h"
#include "UserInputBitset.h"
#include <SFML/Window/Keyboard.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SubscriberFactory::CreateAndRegisterSubscriber creates a subscriber "
          "with no trigger data",
          "[unit][SubscriberFactory]") {
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

TEST_CASE(
    "SubscriberFactory::CreateAndRegisterSubscriber creates a subscriber with "
    "trigger data",
    "[unit][SubscriberFactory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;
  // create a SubscriberFactory with the mock EventHandler
  steamrot::SubscriberFactory factory(mock_event_handler);
  // set up variables for the test
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;

  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::W;
  const steamrot::EventData trigger_data =
      steamrot::UserInputBitset{{key_event}};

  // check that the EventHandler UserInput register is empty initially
  auto &subscriber_register = mock_event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());
  // create and register a Subscriber
  auto create_result =
      factory.CreateAndRegisterSubscriber(event_type, trigger_data);
  if (!create_result.has_value())
    FAIL(create_result.error().message);
  // check that the Subscriber was created successfully
  REQUIRE(!subscriber_register.empty());
  REQUIRE(subscriber_register.size() == 1);
  REQUIRE(subscriber_register.at(event_type).size() == 1);
  REQUIRE(create_result.value() ==
          subscriber_register.at(event_type)[0].lock());
}

TEST_CASE("SubscriberFactory::CreateAndRegisterSubscriber creates a subscriber "
          "from SubscriberConfig with no trigger data",
          "[unit][SubscriberFactory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // create a SubscriberFactory with the mock EventHandler
  steamrot::SubscriberFactory factory(mock_event_handler);

  // set up SubscriberConfig
  steamrot::SubscriberConfig config;
  config.trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  config.active = false;

  // check that the EventHandler register is empty initially
  auto &subscriber_register = mock_event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());

  // create and register a Subscriber from config
  auto create_result = factory.CreateAndRegisterSubscriber(config);
  if (!create_result.has_value())
    FAIL(create_result.error().message);

  // check that the Subscriber was created successfully
  REQUIRE(!subscriber_register.empty());
  REQUIRE(subscriber_register.size() == 1);
  REQUIRE(subscriber_register.at(config.trigger_event_type).size() == 1);
  REQUIRE(create_result.value() ==
          subscriber_register.at(config.trigger_event_type)[0].lock());
  REQUIRE(create_result.value()->m_active == false);
}

TEST_CASE("SubscriberFactory::CreateAndRegisterSubscriber creates a subscriber "
          "from SubscriberConfig with trigger data and active flag",
          "[unit][SubscriberFactory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // create a SubscriberFactory with the mock EventHandler
  steamrot::SubscriberFactory factory(mock_event_handler);

  // set up SubscriberConfig with trigger data
  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::W;
  steamrot::EventData trigger_data = steamrot::UserInputBitset{{key_event}};

  steamrot::SubscriberConfig config;
  config.trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  config.trigger_event_data = trigger_data;
  config.active = true;

  // check that the EventHandler register is empty initially
  auto &subscriber_register = mock_event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());

  // create and register a Subscriber from config
  auto create_result = factory.CreateAndRegisterSubscriber(config);
  if (!create_result.has_value())
    FAIL(create_result.error().message);

  // check that the Subscriber was created successfully
  REQUIRE(!subscriber_register.empty());
  REQUIRE(subscriber_register.size() == 1);
  REQUIRE(subscriber_register.at(config.trigger_event_type).size() == 1);
  REQUIRE(create_result.value() ==
          subscriber_register.at(config.trigger_event_type)[0].lock());
  REQUIRE(create_result.value()->m_active == true);
  REQUIRE(create_result.value()->m_trigger_event_data.has_value());
}
