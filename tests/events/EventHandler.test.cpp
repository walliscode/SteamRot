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
  event_sf.scancode = sf::Keyboard::Scancode::A;
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
TEST_CASE("EventHandler::UpdateSubscribersFromGlobalEventBus updates correct "
          "subscribers",
          "[EventHandler]") {
  sf::Event::KeyPressed event_sf;
  event_sf.scancode = sf::Keyboard::Scancode::A;
  sf::Event event{event_sf};

  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // create Subscriber variables
  const steamrot::EventType event_type = steamrot::EventType_EVENT_USER_INPUT;
  const steamrot::UserInputBitset user_input_bitset{{event}};

  // Create a couple of Subscriber instances
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type);
  std::shared_ptr<steamrot::Subscriber> subscriber2 =
      std::make_shared<steamrot::Subscriber>(event_type);

  // check that the subscriber is not active
  REQUIRE(!subscriber->IsActive());
  REQUIRE(!subscriber2->IsActive());
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    FAIL(result.error().message);
  auto result2 = event_handler.RegisterSubscriber(subscriber2);
  if (!result2.has_value())
    FAIL(result2.error().message);

  // Create some EventPackets to add
  steamrot::EventPacket event1{2};
  event1.m_event_type = event_type;
  event1.m_event_data = user_input_bitset;

  std::vector<steamrot::EventPacket> events_to_add = {event1};
  // Add events to the global event bus
  event_handler.AddToGlobalEventBus(events_to_add);
  // Update subscribers from the global event bus
  event_handler.UpateSubscribersFromGlobalEventBus();
  // check that the subscriber is now active
  REQUIRE(subscriber->IsActive());
  REQUIRE(subscriber2->IsActive());
}

TEST_CASE("EventHandler: trigger event prevents activation when condition not met", "[EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  
  // create test data
  const steamrot::EventType event_type = steamrot::EventType::EventType_EVENT_TOGGLE_DROPDOWN;
  const steamrot::EventData trigger_data = std::string("specific_dropdown");
  const steamrot::EventData different_data = std::string("other_dropdown");
  
  // Create subscriber with trigger condition
  std::shared_ptr<steamrot::Subscriber> subscriber_with_trigger =
      std::make_shared<steamrot::Subscriber>(event_type, trigger_data);
  
  // Register the subscriber
  auto result = event_handler.RegisterSubscriber(subscriber_with_trigger);
  REQUIRE(result.has_value());
  REQUIRE(!subscriber_with_trigger->IsActive());
  
  // Create event with different data that doesn't match trigger
  steamrot::EventPacket event_no_match{1};
  event_no_match.m_event_type = event_type;
  event_no_match.m_event_data = different_data;
  
  std::vector<steamrot::EventPacket> events_no_match = {event_no_match};
  event_handler.AddToGlobalEventBus(events_no_match);
  event_handler.UpateSubscribersFromGlobalEventBus();
  
  // Subscriber should NOT be activated due to trigger mismatch
  REQUIRE(!subscriber_with_trigger->IsActive());
}

TEST_CASE("EventHandler: trigger event allows activation when condition is met", "[EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  
  // create test data
  const steamrot::EventType event_type = steamrot::EventType::EventType_EVENT_TOGGLE_DROPDOWN;
  const steamrot::EventData trigger_data = std::string("specific_dropdown");
  
  // Create subscriber with trigger condition
  std::shared_ptr<steamrot::Subscriber> subscriber_with_trigger =
      std::make_shared<steamrot::Subscriber>(event_type, trigger_data);
  
  // Register the subscriber
  auto result = event_handler.RegisterSubscriber(subscriber_with_trigger);
  REQUIRE(result.has_value());
  REQUIRE(!subscriber_with_trigger->IsActive());
  
  // Create event with matching data
  steamrot::EventPacket event_match{1};
  event_match.m_event_type = event_type;
  event_match.m_event_data = trigger_data;
  
  std::vector<steamrot::EventPacket> events_match = {event_match};
  event_handler.AddToGlobalEventBus(events_match);
  event_handler.UpateSubscribersFromGlobalEventBus();
  
  // Subscriber should be activated due to trigger match
  REQUIRE(subscriber_with_trigger->IsActive());
  REQUIRE(subscriber_with_trigger->GetEventData() == trigger_data);
}

TEST_CASE("EventHandler: multiple subscribers with different trigger conditions", "[EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  
  // create test data
  const steamrot::EventType event_type = steamrot::EventType::EventType_EVENT_TOGGLE_DROPDOWN;
  const steamrot::EventData trigger_data1 = std::string("dropdown_1");
  const steamrot::EventData trigger_data2 = std::string("dropdown_2");
  
  // Create subscribers with different trigger conditions
  std::shared_ptr<steamrot::Subscriber> subscriber1 =
      std::make_shared<steamrot::Subscriber>(event_type, trigger_data1);
  std::shared_ptr<steamrot::Subscriber> subscriber2 =
      std::make_shared<steamrot::Subscriber>(event_type, trigger_data2);
  std::shared_ptr<steamrot::Subscriber> subscriber_no_trigger =
      std::make_shared<steamrot::Subscriber>(event_type);
  
  // Register all subscribers
  auto result1 = event_handler.RegisterSubscriber(subscriber1);
  auto result2 = event_handler.RegisterSubscriber(subscriber2);
  auto result3 = event_handler.RegisterSubscriber(subscriber_no_trigger);
  REQUIRE(result1.has_value());
  REQUIRE(result2.has_value());
  REQUIRE(result3.has_value());
  REQUIRE(!subscriber1->IsActive());
  REQUIRE(!subscriber2->IsActive());
  REQUIRE(!subscriber_no_trigger->IsActive());
  
  // Create event that matches only subscriber1's trigger
  steamrot::EventPacket event1{1};
  event1.m_event_type = event_type;
  event1.m_event_data = trigger_data1;
  
  std::vector<steamrot::EventPacket> events1 = {event1};
  event_handler.AddToGlobalEventBus(events1);
  event_handler.UpateSubscribersFromGlobalEventBus();
  
  // Only subscriber1 and subscriber_no_trigger should be activated
  REQUIRE(subscriber1->IsActive());
  REQUIRE(!subscriber2->IsActive());
  REQUIRE(subscriber_no_trigger->IsActive());
  
  // Reset states
  subscriber1->SetInactive();
  subscriber_no_trigger->SetInactive();
  
  // Create event that matches subscriber2's trigger
  steamrot::EventPacket event2{1};
  event2.m_event_type = event_type;
  event2.m_event_data = trigger_data2;
  
  // Clear previous events and add new ones
  event_handler.TickGlobalEventBus(); // Remove old events
  std::vector<steamrot::EventPacket> events2 = {event2};
  event_handler.AddToGlobalEventBus(events2);
  event_handler.UpateSubscribersFromGlobalEventBus();
  
  // Only subscriber2 and subscriber_no_trigger should be activated
  REQUIRE(!subscriber1->IsActive());
  REQUIRE(subscriber2->IsActive());
  REQUIRE(subscriber_no_trigger->IsActive());
}
