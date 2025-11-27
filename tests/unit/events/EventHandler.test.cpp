/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the EventHandler class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "EventPacket.h"
#include "Subscriber.h"
#include "events_generated.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <X11/extensions/XTest.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EventHandler registers Subscribers", "[unit][EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;

  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_QUIT_GAME;

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type);

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
  REQUIRE(event_bus[0].event_lifetime == 2);
  REQUIRE(event_bus[1].event_lifetime == 0);
  REQUIRE(event_bus[2].event_lifetime == 1);
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
  REQUIRE(event_bus[0].event_lifetime == 3);
  REQUIRE(event_bus[1].event_lifetime == 1);
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
  REQUIRE(global_event_bus[0].event_lifetime == 3);
  REQUIRE(global_event_bus[1].event_lifetime == 2);
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
  REQUIRE(global_event_bus[0].event_lifetime == 2);
  REQUIRE(global_event_bus[1].event_lifetime == 1);
  // Tick the global event bus to update lifetimes and remove dead events
  event_handler.TickGlobalEventBus();
  global_event_bus = event_handler.GetGlobalEventBus();
  REQUIRE(global_event_bus.size() == 1);
  REQUIRE(global_event_bus[0].event_lifetime == 1);
  // Tick again, which should remove the last event
  event_handler.TickGlobalEventBus();
  global_event_bus = event_handler.GetGlobalEventBus();
  REQUIRE(global_event_bus.empty());
}

TEST_CASE("EventHandler::ResetAllSubscribers resets all subscribers to inactive",
          "[unit][EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;

  // Create subscribers with different event types
  const steamrot::EventType event_type1 =
      steamrot::EventType::EventType_EVENT_USER_INPUT;
  const steamrot::EventType event_type2 =
      steamrot::EventType::EventType_EVENT_QUIT_GAME;

  std::shared_ptr<steamrot::Subscriber> subscriber1 =
      std::make_shared<steamrot::Subscriber>(event_type1);
  std::shared_ptr<steamrot::Subscriber> subscriber2 =
      std::make_shared<steamrot::Subscriber>(event_type2);

  // Register subscribers
  auto result1 = event_handler.RegisterSubscriber(subscriber1);
  REQUIRE(result1.has_value());
  auto result2 = event_handler.RegisterSubscriber(subscriber2);
  REQUIRE(result2.has_value());

  // Manually set subscribers to active
  subscriber1->m_active = true;
  subscriber2->m_active = true;
  REQUIRE(subscriber1->m_active);
  REQUIRE(subscriber2->m_active);

  // Reset all subscribers
  event_handler.ResetAllSubscribers();

  // Check that all subscribers are now inactive
  REQUIRE_FALSE(subscriber1->m_active);
  REQUIRE_FALSE(subscriber2->m_active);
}

TEST_CASE("EventHandler::TickGlobalEventBus resets all subscribers",
          "[unit][EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;

  // Create and register a subscriber
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type);

  auto result = event_handler.RegisterSubscriber(subscriber);
  REQUIRE(result.has_value());

  // Manually set subscriber to active
  subscriber->m_active = true;
  REQUIRE(subscriber->m_active);

  // Tick the global event bus (which should also reset subscribers)
  event_handler.TickGlobalEventBus();

  // Check that the subscriber is now inactive
  REQUIRE_FALSE(subscriber->m_active);
}

TEST_CASE("UpdateSubscribers turns on Subscribers",
          "[unit][EventHandler]") {

  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type);

  // check that the subscriber is not active
  REQUIRE(!subscriber->m_active);

  // create Event Data to use
  sf::Event::KeyPressed event_sf;
  event_sf.code = sf::Keyboard::Key::A;
  sf::Event event{event_sf};
  steamrot::EventData user_input_bitset;
  user_input_bitset = steamrot::UserInputBitset{{event}};

  // Update the subscriber with the event data
  std::weak_ptr<steamrot::Subscriber> weak_subscriber = subscriber;
  steamrot::UpdateSubscriber(weak_subscriber, user_input_bitset);

  REQUIRE(subscriber->m_active);
}

TEST_CASE("EventHandler::UpdateSubscribersFrom does not update Subscribers "
          "when events do not match",
          "[unit][EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type);

  // check that the subscriber is not active
  REQUIRE(!subscriber->m_active);
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    FAIL(result.error().message);

  // Create some EventPackets to add that do not match the subscriber
  steamrot::EventPacket event1{2};
  event1.m_event_type = steamrot::EventType::EventType_EVENT_TEST;
  std::vector<steamrot::EventPacket> events_to_add = {event1};

  // Add events to the global event bus
  for (const auto &event : events_to_add)
    event_handler.AddEvent(event);

  // Update subscribers from the global event bus
  event_handler.UpateSubscribersFromGlobalEventBus();

  // check that the subscriber is still not active
  REQUIRE(!subscriber->m_active);
}

TEST_CASE("EventHandler::UpdateSubscribers does not update Subscriber if "
          "trigger_data is present and does not match",
          "[unit][EventHandler]") {
  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;

  // Create a Subscriber instance with trigger data
  sf::Event::KeyPressed event_sf;
  event_sf.code = sf::Keyboard::Key::A;

  sf::Event event{event_sf};
  steamrot::EventData trigger_data;
  trigger_data = steamrot::UserInputBitset{{event}};
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type, trigger_data);

  // check that the subscriber is not active
  REQUIRE(!subscriber->m_active);

  // create Event Data to copy that does not match the trigger data
  sf::Event::KeyPressed event_sf2;
  event_sf2.code = sf::Keyboard::Key::B;
  sf::Event event2{event_sf2};
  steamrot::EventData user_input_bitset;
  user_input_bitset = steamrot::UserInputBitset{{event2}};

  REQUIRE(user_input_bitset != trigger_data);
  // Update the subscriber with the event data
  std::weak_ptr<steamrot::Subscriber> weak_subscriber = subscriber;
  steamrot::UpdateSubscriber(weak_subscriber, user_input_bitset);

  // check that the subscriber is still not active
  REQUIRE_FALSE(subscriber->m_active);
}

TEST_CASE(
    "EventHandler::UpdateSubscribers activates Subscriber if trigger_data "
    "is present and matches",
    "[unit][EventHandler]") {
  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;

  // Create a Subscriber instance with trigger data
  sf::Event::KeyPressed event_sf;
  event_sf.code = sf::Keyboard::Key::A;
  sf::Event event{event_sf};
  steamrot::EventData trigger_data;
  trigger_data = steamrot::UserInputBitset{{event}};
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type, trigger_data);

  // check that the subscriber is not active
  REQUIRE(!subscriber->m_active);

  // create Event Data to copy that matches the trigger data
  steamrot::EventData user_input_bitset;
  user_input_bitset = steamrot::UserInputBitset{{event}};
  REQUIRE(user_input_bitset == trigger_data);

  // Update the subscriber with the event data
  std::weak_ptr<steamrot::Subscriber> weak_subscriber = subscriber;
  steamrot::UpdateSubscriber(weak_subscriber, user_input_bitset);

  // check that the subscriber is now active
  REQUIRE(subscriber->m_active);
}
TEST_CASE("EventHandler::UpdateSubscribersFromGlobalEventBus updates correct "
          "subscribers, with and without trigger data",
          "[unit][EventHandler]") {
  sf::Event::KeyPressed event_sf;
  event_sf.code = sf::Keyboard::Key::A;
  sf::Event event{event_sf};

  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  const steamrot::EventType event_type = steamrot::EventType_EVENT_USER_INPUT;
  const steamrot::UserInputBitset user_input_bitset{{event}};

  // Subscriber with NO trigger data (should always activate if event matches)
  std::shared_ptr<steamrot::Subscriber> subscriber_no_trigger =
      std::make_shared<steamrot::Subscriber>(event_type);

  // Subscriber WITH trigger data that matches the event (should activate)
  steamrot::EventData matching_trigger_data = user_input_bitset;
  std::shared_ptr<steamrot::Subscriber> subscriber_trigger_match =
      std::make_shared<steamrot::Subscriber>(event_type, matching_trigger_data);

  // Subscriber WITH trigger data that does NOT match the event (should NOT
  // activate)
  sf::Event::KeyPressed event_sf2;
  event_sf2.code = sf::Keyboard::Key::B;
  sf::Event event2{event_sf2};
  steamrot::EventData nonmatching_trigger_data =
      steamrot::UserInputBitset{{event2}};
  std::shared_ptr<steamrot::Subscriber> subscriber_trigger_no_match =
      std::make_shared<steamrot::Subscriber>(event_type,
                                             nonmatching_trigger_data);

  // All should be inactive to start
  REQUIRE(!subscriber_no_trigger->m_active);
  REQUIRE(!subscriber_trigger_match->m_active);
  REQUIRE(!subscriber_trigger_no_match->m_active);

  // Register all 3
  auto result_no_trigger =
      event_handler.RegisterSubscriber(subscriber_no_trigger);
  if (!result_no_trigger.has_value())
    FAIL(result_no_trigger.error().message);
  auto result_trigger_match =
      event_handler.RegisterSubscriber(subscriber_trigger_match);
  if (!result_trigger_match.has_value())
    FAIL(result_trigger_match.error().message);
  auto result_trigger_no_match =
      event_handler.RegisterSubscriber(subscriber_trigger_no_match);
  if (!result_trigger_no_match.has_value())
    FAIL(result_trigger_no_match.error().message);

  // Add one EventPacket with matching event data
  steamrot::EventPacket event1{2};
  event1.m_event_type = event_type;
  event1.m_event_data = user_input_bitset;
  std::vector<steamrot::EventPacket> events_to_add = {event1};
  for (const auto &event : events_to_add)
    event_handler.AddEvent(event);

  event_handler.ProcessWaitingRoomEventBus();
  // Update subscribers from the global event bus
  event_handler.UpateSubscribersFromGlobalEventBus();

  // Subscriber with no trigger data should be active
  REQUIRE(subscriber_no_trigger->m_active);
  // Subscriber with matching trigger data should be active
  REQUIRE(subscriber_trigger_match->m_active);
  // Subscriber with non-matching trigger data should NOT be active
  REQUIRE_FALSE(subscriber_trigger_no_match->m_active);
}
