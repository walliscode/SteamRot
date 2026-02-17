/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the EventHandler class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "EventPacket.h"
#include "EventPayload.h"
#include "EventType.h"
#include "Subscriber.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <X11/extensions/XTest.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EventHandler registers Subscribers", "[unit][EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;

  // create Subscriber variables
  const steamrot::EventType event_type = steamrot::EventType::SYSTEM;

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>();
  subscriber->event_type = event_type;

  // Check that the EventHandler UserInput register is empty initially
  auto subscriber_register = event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.empty());

  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    FAIL(result.error().message);

  // Check that the Subscriber was registered successfully
  subscriber_register = event_handler.GetSubcriberRegister();

  REQUIRE(subscriber_register.size() == 1);
  REQUIRE(subscriber_register.at(event_type).size() == 1);

  // check that the pointers are the same
  REQUIRE(subscriber == subscriber_register.at(event_type)[0].lock());

  // when the subscriber goes out of scope, the weak pointer should be expired
  subscriber.reset();
  REQUIRE(subscriber_register.at(event_type)[0].expired());
}
TEST_CASE("AddEvent adds an event to an EventBus", "[unit][EventHandler]") {
  // create an eventHandler
  steamrot::EventHandler event_handler;
  REQUIRE(event_handler.GetGlobalEventBus().empty());

  // create an event and add it to the bus
  steamrot::EventPacket event{5};
  event_handler.AddEvent(event);

  event_handler.ProcessWaitingRoomEventBus();
  // check that the event was added
  REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
}

TEST_CASE("DecrementEventLifetimes decrease all lifetimes by 1",
          "[unit][EventHandler]") {

  // create a mock EventBus with events of varying lifetimes
  steamrot::EventBus event_bus;
  steamrot::EventPacket event1{3};
  steamrot::EventPacket event2{1};
  steamrot::EventPacket event3{2};

  // add events to the bus
  event_bus.push_back(event1);
  event_bus.push_back(event2);
  event_bus.push_back(event3);

  REQUIRE(event_bus.size() == 3);

  // decrement lifetimes
  steamrot::DecrementEventLifetimes(event_bus);
  REQUIRE(event_bus[0].context.lifetime == 2);
  REQUIRE(event_bus[1].context.lifetime == 0);
  REQUIRE(event_bus[2].context.lifetime == 1);
}

TEST_CASE("RemoveDeadEvents removes dead events", "[unit][EventHandler]") {
  // create a mock EventBus with events of varying lifetimes
  steamrot::EventBus event_bus;
  steamrot::EventPacket event1{3};
  steamrot::EventPacket event2{0}; // dead event
  steamrot::EventPacket event3{1};
  // add events to the bus
  event_bus.push_back(event1);
  event_bus.push_back(event2);
  event_bus.push_back(event3);

  REQUIRE(event_bus.size() == 3);
  // remove dead events
  steamrot::RemoveDeadEvents(event_bus);
  REQUIRE(event_bus.size() == 2);
  REQUIRE(event_bus[0].context.lifetime == 3);
  REQUIRE(event_bus[1].context.lifetime == 1);
}

TEST_CASE(
    "EventHandler::AddToGlobalEventBus adds events to the global event bus",
    "[unit][EventHandler]") {

  // Create an EventHandler instance
  steamrot::EventHandler event_handler;

  // Check that the global event bus is empty initially
  auto global_event_bus = event_handler.GetGlobalEventBus();
  REQUIRE(global_event_bus.empty());

  // Create some EventPackets to add
  steamrot::EventPacket event1{3};
  steamrot::EventPacket event2{2};
  std::vector<steamrot::EventPacket> events_to_add = {event1, event2};

  // Add events to the global event bus
  for (const auto &event : events_to_add)
    event_handler.AddEvent(event);

  event_handler.ProcessWaitingRoomEventBus();
  // Check that the events were added successfully
  global_event_bus = event_handler.GetGlobalEventBus();

  REQUIRE(global_event_bus.size() == 2);
  REQUIRE(global_event_bus[0].context.lifetime == 3);
  REQUIRE(global_event_bus[1].context.lifetime == 2);
}

TEST_CASE("HandleSFMLEvents adds events to anEventBus") {

  // create render window and event bus
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Test Window");
  steamrot::EventBus event_bus;
}

TEST_CASE("EventHandler::TickGlobalEventBus updates the global event bus",
          "[unit][EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // Create some EventPackets to add
  steamrot::EventPacket event1{2};
  steamrot::EventPacket event2{1};
  std::vector<steamrot::EventPacket> events_to_add = {event1, event2};
  // Add events to the global event bus
  for (const auto &event : events_to_add)
    event_handler.AddEvent(event);
  event_handler.ProcessWaitingRoomEventBus();
  // Check that the events were added successfully
  auto global_event_bus = event_handler.GetGlobalEventBus();
  REQUIRE(global_event_bus.size() == 2);
  REQUIRE(global_event_bus[0].context.lifetime == 2);
  REQUIRE(global_event_bus[1].context.lifetime == 1);
  // Tick the global event bus to update lifetimes and remove dead events
  event_handler.TickGlobalEventBus();
  global_event_bus = event_handler.GetGlobalEventBus();
  REQUIRE(global_event_bus.size() == 1);
  REQUIRE(global_event_bus[0].context.lifetime == 1);
  // Tick again, which should remove the last event
  event_handler.TickGlobalEventBus();
  global_event_bus = event_handler.GetGlobalEventBus();
  REQUIRE(global_event_bus.empty());
}

TEST_CASE("UpdateSubscribers turns on Subscribers", "[unit][EventHandler]") {

  // create Subscriber variables
  const steamrot::EventType event_type = steamrot::EventType::USER_INPUT;

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>();
  subscriber->event_type = event_type;

  // check that the subscriber is not active
  REQUIRE(!subscriber->m_active);

  // create InputPayload to pass to UpdateSubscribers
  steamrot::EventPayload input_payload{
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT}};

  // Update the subscriber with the event data
  std::weak_ptr<steamrot::Subscriber> weak_subscriber = subscriber;
  steamrot::UpdateSubscriber(weak_subscriber, input_payload);

  REQUIRE(subscriber->m_active);
}

TEST_CASE("EventHandler::UpdateSubscribersFrom does not update Subscribers "
          "when events do not match",
          "[unit][EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // create Subscriber variables
  const steamrot::EventType event_type = steamrot::EventType::USER_INPUT;

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>();
  subscriber->event_type = event_type;

  // check that the subscriber is not active
  REQUIRE(!subscriber->m_active);
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    FAIL(result.error().message);

  // Create some EventPackets to add that do not match the subscriber
  steamrot::EventPacket event1{2};
  event1.type = steamrot::EventType::NONE;
  std::vector<steamrot::EventPacket> events_to_add = {event1};

  // Add events to the global event bus
  for (const auto &event : events_to_add)
    event_handler.AddEvent(event);

  // Update subscribers from the global event bus
  event_handler.UpdateSubscribersFromGlobalEventBus();

  // check that the subscriber is still not active
  REQUIRE(!subscriber->m_active);
}

TEST_CASE("EventHandler::UpdateSubscribers does not update Subscriber if "
          "filter_payload is present and does not match",
          "[unit][EventHandler]") {
  // create Subscriber variables
  const steamrot::EventType event_type = steamrot::EventType::USER_INPUT;

  // Create a Subscriber instance with filter payload
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>();
  subscriber->event_type = event_type;
  // Set filter payload to SELECT
  subscriber->filter_payload = steamrot::EventPayload{
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT}};

  // check that the subscriber is not active
  REQUIRE(!subscriber->m_active);

  // create EventPayload that does not match the filter payload
  steamrot::EventPayload non_matching_payload{
      steamrot::InputPayload{steamrot::InputPayload::InputAction::NONE}};

  // Update the subscriber with the event data
  std::weak_ptr<steamrot::Subscriber> weak_subscriber = subscriber;
  steamrot::UpdateSubscriber(weak_subscriber, non_matching_payload);

  // check that the subscriber is still not active
  REQUIRE_FALSE(subscriber->m_active);
}

TEST_CASE(
    "EventHandler::UpdateSubscribers activates Subscriber if filter_payload "
    "is present and matches",
    "[unit][EventHandler]") {
  // create Subscriber variables
  const steamrot::EventType event_type = steamrot::EventType::USER_INPUT;

  // Create a Subscriber instance with filter payload
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>();
  subscriber->event_type = event_type;
  // Set filter payload to SELECT
  steamrot::EventPayload filter_payload{
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT}};
  subscriber->filter_payload = filter_payload;

  // check that the subscriber is not active
  REQUIRE(!subscriber->m_active);

  // create EventPayload that matches the filter payload
  steamrot::EventPayload matching_payload{
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT}};

  // Update the subscriber with the event data
  std::weak_ptr<steamrot::Subscriber> weak_subscriber = subscriber;
  steamrot::UpdateSubscriber(weak_subscriber, matching_payload);

  // check that the subscriber is now active
  REQUIRE(subscriber->m_active);
}
TEST_CASE("EventHandler::UpdateSubscribersFromGlobalEventBus updates correct "
          "subscribers, with and without filter_payload",
          "[unit][EventHandler]") {

  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  const steamrot::EventType event_type = steamrot::EventType::USER_INPUT;

  // Create EventPayload for INPUT action SELECT
  steamrot::EventPayload input_payload{
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT}};

  // Subscriber with NO filter payload (should always activate if event type matches)
  std::shared_ptr<steamrot::Subscriber> subscriber_no_filter =
      std::make_shared<steamrot::Subscriber>();
  subscriber_no_filter->event_type = event_type;

  // Subscriber WITH filter payload that matches the event (should activate)
  std::shared_ptr<steamrot::Subscriber> subscriber_filter_match =
      std::make_shared<steamrot::Subscriber>();
  subscriber_filter_match->event_type = event_type;
  subscriber_filter_match->filter_payload = input_payload;

  // Subscriber WITH filter payload that does NOT match the event (should NOT
  // activate)
  steamrot::EventPayload nonmatching_filter_payload{
      steamrot::InputPayload{steamrot::InputPayload::InputAction::NONE}};
  std::shared_ptr<steamrot::Subscriber> subscriber_filter_no_match =
      std::make_shared<steamrot::Subscriber>();
  subscriber_filter_no_match->event_type = event_type;
  subscriber_filter_no_match->filter_payload = nonmatching_filter_payload;

  // All should be inactive to start
  REQUIRE(!subscriber_no_filter->m_active);
  REQUIRE(!subscriber_filter_match->m_active);
  REQUIRE(!subscriber_filter_no_match->m_active);

  // Register all 3
  auto result_no_filter =
      event_handler.RegisterSubscriber(subscriber_no_filter);
  if (!result_no_filter.has_value())
    FAIL(result_no_filter.error().message);
  auto result_filter_match =
      event_handler.RegisterSubscriber(subscriber_filter_match);
  if (!result_filter_match.has_value())
    FAIL(result_filter_match.error().message);
  auto result_filter_no_match =
      event_handler.RegisterSubscriber(subscriber_filter_no_match);
  if (!result_filter_no_match.has_value())
    FAIL(result_filter_no_match.error().message);

  // Add one EventPacket with matching event payload
  steamrot::EventPacket event1{2};
  event1.type = event_type;
  event1.payload = input_payload;
  std::vector<steamrot::EventPacket> events_to_add = {event1};
  for (const auto &event : events_to_add)
    event_handler.AddEvent(event);

  event_handler.ProcessWaitingRoomEventBus();
  // Update subscribers from the global event bus
  event_handler.UpdateSubscribersFromGlobalEventBus();

  // Subscriber with no filter payload should be active
  REQUIRE(subscriber_no_filter->m_active);
  // Subscriber with matching filter payload should be active
  REQUIRE(subscriber_filter_match->m_active);
  // Subscriber with non-matching filter payload should NOT be active
  REQUIRE_FALSE(subscriber_filter_no_match->m_active);
}

TEST_CASE("ResetAllSubscribers resets all Subscribers",
          "[unit][EventHandler]") {
  // create Subscriber variables
  const steamrot::EventType event_type1 = steamrot::EventType::USER_INPUT;
  const steamrot::EventType event_type2 = steamrot::EventType::SYSTEM;
  // Create Subscriber instances
  std::shared_ptr<steamrot::Subscriber> subscriber1 =
      std::make_shared<steamrot::Subscriber>();
  subscriber1->event_type = event_type1;
  subscriber1->m_active = true; // set active to true
  std::shared_ptr<steamrot::Subscriber> subscriber2 =
      std::make_shared<steamrot::Subscriber>();
  subscriber2->event_type = event_type2;
  subscriber2->m_active = true; // set active to true
  // Create a subscriber register
  std::unordered_map<steamrot::EventType,
                     std::vector<std::weak_ptr<steamrot::Subscriber>>>
      subscriber_register;
  subscriber_register[event_type1].push_back(subscriber1);
  subscriber_register[event_type2].push_back(subscriber2);
  // Reset all subscribers
  auto result = steamrot::ResetAllSubscribers(subscriber_register);
  if (!result.has_value())
    FAIL(result.error().message);
  // Check that all subscribers are reset
  REQUIRE(!subscriber1->m_active);
  REQUIRE(!subscriber2->m_active);
}

TEST_CASE("EventHandler::CleanExpiredSubscribers removes expired Subscribers",
          "[unit][EventHandler]") {

  // create Subscriber variables
  const steamrot::EventType event_type1 = steamrot::EventType::USER_INPUT;
  const steamrot::EventType event_type2 = steamrot::EventType::SYSTEM;
  // Create Subscriber instances
  std::shared_ptr<steamrot::Subscriber> subscriber1 =
      std::make_shared<steamrot::Subscriber>();
  subscriber1->event_type = event_type1;
  std::shared_ptr<steamrot::Subscriber> subscriber2 =
      std::make_shared<steamrot::Subscriber>();
  subscriber2->event_type = event_type2;
  // Create a subscriber register
  std::unordered_map<steamrot::EventType,
                     std::vector<std::weak_ptr<steamrot::Subscriber>>>
      subscriber_register;
  subscriber_register[event_type1].push_back(subscriber1);
  subscriber_register[event_type2].push_back(subscriber2);
  REQUIRE(subscriber_register[event_type1].size() == 1);
  REQUIRE(subscriber_register[event_type2].size() == 1);
  // Reset shared pointers to expire weak pointers
  subscriber1.reset();
  subscriber2.reset();
  // Clean expired subscribers
  auto result = steamrot::CleanExpiredSubscribers(subscriber_register);
  if (!result.has_value())
    FAIL(result.error().message);
  // Check that all subscribers are removed
  REQUIRE(subscriber_register[event_type1].empty());
  REQUIRE(subscriber_register[event_type2].empty());
}

TEST_CASE("EventHandler::ExecuteEventHandlerLevelLogic resets all Subscribers",
          "[unit][EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // create Subscriber variables
  const steamrot::EventType event_type = steamrot::EventType::USER_INPUT;
  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>();
  subscriber->event_type = event_type;
  subscriber->m_active = true; // set active to true
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    FAIL(result.error().message);
  // Create some EventPackets to add
  steamrot::EventPacket event1{2};
  event1.type = steamrot::EventType::LOGIC;
  std::vector<steamrot::EventPacket> events_to_add = {event1};
  // Add events to the global event bus
  for (const auto &event : events_to_add)
    event_handler.AddEvent(event);
  // Execute EventHandler level logic
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Test Window");
  event_handler.ExecuteEventHandlerLevelLogic(window);
  // Check that the subscriber is reset
  REQUIRE(!subscriber->m_active);
}

TEST_CASE(
    "EventHandler::ExecuteEventHandlerLevelLogic removes dead Subscribers",
    "[unit][EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // create Subscriber variables
  const steamrot::EventType event_type = steamrot::EventType::USER_INPUT;
  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>();
  subscriber->event_type = event_type;
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    FAIL(result.error().message);
  // Reset shared pointer to expire weak pointer
  subscriber.reset();
  // Execute EventHandler level logic
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Test Window");
  event_handler.ExecuteEventHandlerLevelLogic(window);
  // Check that the subscriber register is empty
  auto subscriber_register = event_handler.GetSubcriberRegister();
  REQUIRE(subscriber_register.at(event_type).empty());
}
