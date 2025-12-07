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
          "from Subscriber vector",
          "[unit][subscriber_factory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // set up Subscriber vector
  std::vector<steamrot::Subscriber> subscribers_input;
  
  steamrot::Subscriber sub1;
  sub1.m_trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  sub1.m_active = false;
  subscribers_input.push_back(sub1);

  steamrot::Subscriber sub2;
  sub2.m_trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::W;
  sub2.m_trigger_event_data = steamrot::UserInputBitset{{key_event}};
  sub2.m_active = true;
  subscribers_input.push_back(sub2);

  // check that the EventHandler register is empty initially
  auto &subscriber_register = mock_event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());

  // Vector to store created subscribers
  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers_output;

  // create and register subscribers
  auto result = steamrot::subscriber_factory::CreateAndRegisterSubscribers(
      subscribers_input, subscribers_output, mock_event_handler);
  
  if (!result.has_value())
    FAIL(result.error().message);

  // check that the Subscribers were created successfully
  REQUIRE(!subscriber_register.empty());
  REQUIRE(subscriber_register.size() == 1);
  REQUIRE(subscriber_register.at(sub1.m_trigger_event_type).size() == 2);
  REQUIRE(subscribers_output.size() == 2);
  REQUIRE(subscribers_output[0]->m_active == false);
  REQUIRE(subscribers_output[1]->m_active == true);
  REQUIRE(subscribers_output[1]->m_trigger_event_data.has_value());
}

TEST_CASE("subscriber_factory::CreateAndRegisterSubscribers skips NONE event types",
          "[unit][subscriber_factory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // set up Subscriber vector with NONE type
  std::vector<steamrot::Subscriber> subscribers_input;
  
  steamrot::Subscriber sub1;
  sub1.m_trigger_event_type = steamrot::EventType::EventType_NONE;
  subscribers_input.push_back(sub1);

  steamrot::Subscriber sub2;
  sub2.m_trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  subscribers_input.push_back(sub2);

  // check that the EventHandler register is empty initially
  auto &subscriber_register = mock_event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());

  // Vector to store created subscribers
  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers_output;

  // create and register subscribers
  auto result = steamrot::subscriber_factory::CreateAndRegisterSubscribers(
      subscribers_input, subscribers_output, mock_event_handler);
  
  if (!result.has_value())
    FAIL(result.error().message);

  // check that only one subscriber was created (the NONE was skipped)
  REQUIRE(!subscriber_register.empty());
  REQUIRE(subscribers_output.size() == 1);
}

TEST_CASE("subscriber_factory::CreateAndRegisterSubscribers handles empty vector",
          "[unit][subscriber_factory]") {
  // create a mock EventHandler
  steamrot::EventHandler mock_event_handler;

  // empty subscriber vector
  std::vector<steamrot::Subscriber> subscribers_input;

  // Vector to store created subscribers
  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers_output;

  // create and register subscribers
  auto result = steamrot::subscriber_factory::CreateAndRegisterSubscribers(
      subscribers_input, subscribers_output, mock_event_handler);
  
  if (!result.has_value())
    FAIL(result.error().message);

  // check that no subscribers were created
  REQUIRE(subscribers_output.empty());
}
