/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the subscriber_factory namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "subscriber_factory.h"
#include "EventHandler.h"
#include "UserInputBitset.h"
#include <SFML/Window/Keyboard.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("subscriber_factory::CreateAndRegisterSubscribers creates subscribers "
          "from SubscriberConfig vector",
          "[unit][subscriber_factory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // set up SubscriberConfig vector
  std::vector<steamrot::SubscriberConfig> configs;
  
  steamrot::SubscriberConfig config1;
  config1.trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  config1.active = false;
  configs.push_back(config1);

  steamrot::SubscriberConfig config2;
  config2.trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::W;
  config2.trigger_event_data = steamrot::UserInputBitset{{key_event}};
  config2.active = true;
  configs.push_back(config2);

  // check that the EventHandler register is empty initially
  auto &subscriber_register = mock_event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());

  // Vector to store created subscribers
  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers;

  // create and register subscribers
  auto result = steamrot::subscriber_factory::CreateAndRegisterSubscribers(
      configs, subscribers, mock_event_handler);
  
  if (!result.has_value())
    FAIL(result.error().message);

  // check that the Subscribers were created successfully
  REQUIRE(!subscriber_register.empty());
  REQUIRE(subscriber_register.size() == 1);
  REQUIRE(subscriber_register.at(config1.trigger_event_type).size() == 2);
  REQUIRE(subscribers.size() == 2);
  REQUIRE(subscribers[0]->m_active == false);
  REQUIRE(subscribers[1]->m_active == true);
  REQUIRE(subscribers[1]->m_trigger_event_data.has_value());
}

TEST_CASE("subscriber_factory::CreateAndRegisterSubscribers skips NONE event types",
          "[unit][subscriber_factory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // set up SubscriberConfig vector with NONE type
  std::vector<steamrot::SubscriberConfig> configs;
  
  steamrot::SubscriberConfig config1;
  config1.trigger_event_type = steamrot::EventType::EventType_NONE;
  configs.push_back(config1);

  steamrot::SubscriberConfig config2;
  config2.trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  configs.push_back(config2);

  // check that the EventHandler register is empty initially
  auto &subscriber_register = mock_event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());

  // Vector to store created subscribers
  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers;

  // create and register subscribers
  auto result = steamrot::subscriber_factory::CreateAndRegisterSubscribers(
      configs, subscribers, mock_event_handler);
  
  if (!result.has_value())
    FAIL(result.error().message);

  // check that only one subscriber was created (the NONE was skipped)
  REQUIRE(!subscriber_register.empty());
  REQUIRE(subscribers.size() == 1);
}

TEST_CASE("subscriber_factory::CreateAndRegisterSubscribers handles empty vector",
          "[unit][subscriber_factory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // empty config vector
  std::vector<steamrot::SubscriberConfig> configs;

  // Vector to store created subscribers
  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers;

  // create and register subscribers
  auto result = steamrot::subscriber_factory::CreateAndRegisterSubscribers(
      configs, subscribers, mock_event_handler);
  
  if (!result.has_value())
    FAIL(result.error().message);

  // check that no subscribers were created
  REQUIRE(subscribers.empty());
}
