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

TEST_CASE("UpdateSubscribers notifies subscribers of events",
          "[EventHandler]") {

  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;
  const steamrot::UserInputBitset user_input_bitset{2};

  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type, user_input_bitset);
  // check that the subscriber is not active
  REQUIRE(!subscriber->IsActive());

  std::weak_ptr<steamrot::Subscriber> weak_subscriber = subscriber;
  steamrot::UpdateSubscriber(weak_subscriber);

  REQUIRE(subscriber->IsActive());
}

TEST_CASE("EventHandler::UpdateSubscribersFrom does not update Subscribers "
          "when events do not match",
          "[EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;
  const steamrot::UserInputBitset user_input_bitset{2};
  // Create a Subscriber instance
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type, user_input_bitset);
  // check that the subscriber is not active
  REQUIRE(!subscriber->IsActive());
  auto result = event_handler.RegisterSubscriber(subscriber);
  if (!result.has_value())
    FAIL(result.error().message);

  // Create some EventPackets to add that do not match the subscriber
  steamrot::EventPacket event1{2};
  event1.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  event1.m_event_data = steamrot::UserInputBitset{3};
  std::vector<steamrot::EventPacket> events_to_add = {event1};
  // Add events to the global event bus
  event_handler.AddToGlobalEventBus(events_to_add);
  // Update subscribers from the global event bus
  event_handler.UpateSubscribersFromGlobalEventBus();
  // check that the subscriber is still not active
  REQUIRE(!subscriber->IsActive());
}
TEST_CASE("EventHandler::UpdateSubscribersFromGlobalEventBus updates correct "
          "subsribers",
          "[EventHandler]") {
  // Create an EventHandler instance
  steamrot::EventHandler event_handler;
  // create Subscriber variables
  const steamrot::EventType event_type =
      steamrot::EventType::EventType_EVENT_USER_INPUT;
  const steamrot::UserInputBitset user_input_bitset{2};
  // Create a couple of Subscriber instances
  std::shared_ptr<steamrot::Subscriber> subscriber =
      std::make_shared<steamrot::Subscriber>(event_type, user_input_bitset);
  std::shared_ptr<steamrot::Subscriber> subscriber2 =
      std::make_shared<steamrot::Subscriber>(event_type, user_input_bitset);
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
