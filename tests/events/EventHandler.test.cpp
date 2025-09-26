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
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("EventHandler registers Subscribers", "[EventHandler]") {
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
TEST_CASE("AddEvent adds an event to an EventBus", "[EventHandler]") {
  // create a mock EventBus
  steamrot::EventBus event_bus;
  REQUIRE(event_bus.empty());

  // create an event and add it to the bus
  steamrot::EventPacket event{5};
  steamrot::AddEvent(event_bus, event);

  // check that the event was added
  REQUIRE(event_bus.size() == 1);
}

TEST_CASE("DecrementEventLifetimes decrease all lifetimes by 1",
          "[EventHandler]") {

  // create a mock EventBus with events of varying lifetimes
  steamrot::EventBus event_bus;
  steamrot::EventPacket event1{3};
  steamrot::EventPacket event2{1};
  steamrot::EventPacket event3{2};

  // add events to the bus
  AddEvent(event_bus, event1);
  AddEvent(event_bus, event2);
  AddEvent(event_bus, event3);
  REQUIRE(event_bus.size() == 3);

  // decrement lifetimes
  steamrot::DecrementEventLifetimes(event_bus);
  REQUIRE(event_bus[0].event_lifetime == 2);
  REQUIRE(event_bus[1].event_lifetime == 0);
  REQUIRE(event_bus[2].event_lifetime == 1);
}

TEST_CASE("RemoveDeadEvents removes dead events", "[EventHandler]") {
  // create a mock EventBus with events of varying lifetimes
  steamrot::EventBus event_bus;
  steamrot::EventPacket event1{3};
  steamrot::EventPacket event2{0}; // dead event
  steamrot::EventPacket event3{1};
  // add events to the bus
  AddEvent(event_bus, event1);
  AddEvent(event_bus, event2);
  AddEvent(event_bus, event3);
  REQUIRE(event_bus.size() == 3);
  // remove dead events
  steamrot::RemoveDeadEvents(event_bus);
  REQUIRE(event_bus.size() == 2);
  REQUIRE(event_bus[0].event_lifetime == 3);
  REQUIRE(event_bus[1].event_lifetime == 1);
}

TEST_CASE(
    "EventHandler::AddToGlobalEventBus adds events to the global event bus",
    "[EventHandler]") {

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
  event_handler.AddToGlobalEventBus(events_to_add);

  // Check that the events were added successfully
  global_event_bus = event_handler.GetGlobalEventBus();

  REQUIRE(global_event_bus.size() == 2);
  REQUIRE(global_event_bus[0].event_lifetime == 3);
  REQUIRE(global_event_bus[1].event_lifetime == 2);
}

TEST_CASE("EventHandler::TickGlobalEventBus updates the global event bus",
          "[EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // Create some EventPackets to add
  steamrot::EventPacket event1{2};
  steamrot::EventPacket event2{1};
  std::vector<steamrot::EventPacket> events_to_add = {event1, event2};
  // Add events to the global event bus
  event_handler.AddToGlobalEventBus(events_to_add);
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

TEST_CASE("UpdateSubscribers turns on Subscribers and copies EventData",
          "[EventHandler]") {

  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type);

  // check that the subscriber is not active and that the data is empty
  REQUIRE(!subscriber->IsActive());
  REQUIRE(std::holds_alternative<std::monostate>(
      subscriber->GetRegistrationInfo().second));

  // create Event Data to copy
  sf::Event::KeyPressed event_sf;
  event_sf.code = sf::Keyboard::Key::A;
  sf::Event event{event_sf};
  steamrot::EventData user_input_bitset;
  user_input_bitset = steamrot::UserInputBitset{{event}};

  // Update the subscriber with the event data
  std::weak_ptr<steamrot::Subscriber> weak_subscriber = subscriber;
  steamrot::UpdateSubscriber(weak_subscriber, user_input_bitset);

  REQUIRE(subscriber->IsActive());
  REQUIRE(std::holds_alternative<steamrot::UserInputBitset>(
      subscriber->GetEventData()));
  REQUIRE(subscriber->GetEventData() == user_input_bitset);
}

TEST_CASE("EventHandler::UpdateSubscribersFrom does not update Subscribers "
          "when events do not match",
          "[EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type);

  // check that the subscriber is not active
  REQUIRE(!subscriber->IsActive());
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    FAIL(result.error().message);

  // Create some EventPackets to add that do not match the subscriber
  steamrot::EventPacket event1{2};
  event1.m_event_type = steamrot::EventType::EventType_EVENT_TEST;
  std::vector<steamrot::EventPacket> events_to_add = {event1};

  // Add events to the global event bus
  event_handler.AddToGlobalEventBus(events_to_add);

  // Update subscribers from the global event bus
  event_handler.UpateSubscribersFromGlobalEventBus();

  // check that the subscriber is still not active
  REQUIRE(!subscriber->IsActive());
}

TEST_CASE("EventHandler::UpdateSubscribers does not update Subscriber if "
          "trigger_data is present and does not match",
          "[EventHandler]") {
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
  REQUIRE(!subscriber->IsActive());

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
  REQUIRE_FALSE(subscriber->IsActive());
}

TEST_CASE(
    "EventHandler::UpdateSubscribers activates Subscriber if trigger_data "
    "is present and matches",
    "[EventHandler]") {
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
  REQUIRE(!subscriber->IsActive());

  // create Event Data to copy that matches the trigger data
  steamrot::EventData user_input_bitset;
  user_input_bitset = steamrot::UserInputBitset{{event}};
  REQUIRE(user_input_bitset == trigger_data);

  // Update the subscriber with the event data
  std::weak_ptr<steamrot::Subscriber> weak_subscriber = subscriber;
  steamrot::UpdateSubscriber(weak_subscriber, user_input_bitset);

  // check that the subscriber is now active
  REQUIRE(subscriber->IsActive());
}
TEST_CASE("EventHandler::UpdateSubscribersFromGlobalEventBus updates correct "
          "subscribers, with and without trigger data",
          "[EventHandler]") {
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
  REQUIRE(!subscriber_no_trigger->IsActive());
  REQUIRE(!subscriber_trigger_match->IsActive());
  REQUIRE(!subscriber_trigger_no_match->IsActive());

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
  event_handler.AddToGlobalEventBus(events_to_add);

  // Update subscribers from the global event bus
  event_handler.UpateSubscribersFromGlobalEventBus();

  // Subscriber with no trigger data should be active
  REQUIRE(subscriber_no_trigger->IsActive());
  // Subscriber with matching trigger data should be active
  REQUIRE(subscriber_trigger_match->IsActive());
  // Subscriber with non-matching trigger data should NOT be active
  REQUIRE_FALSE(subscriber_trigger_no_match->IsActive());
}
