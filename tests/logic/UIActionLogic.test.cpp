/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIActionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "EventPacket.h"
#include "Subscriber.h"
#include "TestContext.h"
#include "events_generated.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <variant>

TEST_CASE("UIActionLogic::UIActionLogic Constructor", "[UIActionLogic]") {
  // Create a dummy LogicContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  // Instantiate UIActionLogic
  steamrot::UIActionLogic ui_action_logic(
      test_context.GetLogicContextForTestScene());
  SUCCEED("UIActionLogic instantiated successfully");
}

TEST_CASE(
    "UIActionLogic fails to process button click for EventType_EVENT_QUIT_GAME",
    "[UIActionLogic]") {
  // arrange the UIActionLogic
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  steamrot::UIActionLogic ui_action_logic(
      test_context.GetLogicContextForTestScene());

  // assert that the EventBus has no events
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      0);
  // arrange a button element
  steamrot::ButtonElement button_element;
  ;
  button_element.position = {100.0f, 100.0f};
  button_element.size = {200.0f, 50.0f};
  button_element.is_mouse_over = false;
  button_element.label = "Quit Game";
  steamrot::Subscriber subscriber{steamrot::EventType_EVENT_USER_INPUT};

  button_element.subscription =
      std::make_shared<steamrot::Subscriber>(subscriber);
  steamrot::EventPacket event_packet{steamrot::EventType_EVENT_QUIT_GAME,
                                     std::monostate()};
  button_element.response_event = event_packet;

  REQUIRE(button_element.subscription != nullptr);
  REQUIRE(button_element.response_event != std::nullopt);

  // act - process a button click
  steamrot::ProcessButtonElementActions(
      button_element, test_context.GetGameContext().event_handler);
  ;

  // assert - that the EventBus has no events
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      0);
}

TEST_CASE("UIActionLogic adds Event to EventBus for ButtonElement with a "
          "click/mouse collision",
          "[UIActionLogic]") {

  // arrange the UIActionLogic
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  steamrot::UIActionLogic ui_action_logic(
      test_context.GetLogicContextForTestScene());
  // assert that the EventBus has no events
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      0);
  // arrange a button element
  steamrot::ButtonElement button_element;
  ;
  button_element.position = {100.0f, 100.0f};
  button_element.size = {200.0f, 50.0f};
  button_element.is_mouse_over = true; // simulate mouse over
  button_element.label = "Quit Game";

  // create response packet
  steamrot::EventPacket event_packet{steamrot::EventType_EVENT_QUIT_GAME,
                                     std::monostate()};
  button_element.response_event = event_packet;
  REQUIRE(button_element.response_event != std::nullopt);

  // act - process a button click
  steamrot::ProcessButtonElementActions(
      button_element, test_context.GetGameContext().event_handler);
  ;
  // need to move events from waiting room to global event bus
  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  // assert - that the EventBus has one event
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      1);
  // assert - that the event is of type EventType_EVENT_USER_INPUT
  const auto &event =
      test_context.GetGameContext().event_handler.GetGlobalEventBus().front();

  REQUIRE(steamrot::EnumNameEventType(event.m_event_type) ==
          steamrot::EnumNameEventType(steamrot::EventType_EVENT_QUIT_GAME));
}
TEST_CASE(
    "UIActionLogic does NOT trigger if only unrelated events are on EventBus "
    "[UIActionLogic][Negative][UnrelatedEvent]") {

  // Arrange the UIActionLogic
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  steamrot::UIActionLogic ui_action_logic(
      test_context.GetLogicContextForTestScene());

  // Assert that the EventBus is empty
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      0);

  // Arrange a button element
  steamrot::ButtonElement button_element;
  button_element.position = {100.0f, 100.0f};
  button_element.size = {200.0f, 50.0f};
  button_element.is_mouse_over = true; // simulate mouse over
  button_element.label = "Quit Game";
  steamrot::Subscriber subscriber{steamrot::EventType_EVENT_USER_INPUT};

  button_element.subscription =
      std::make_shared<steamrot::Subscriber>(subscriber);

  // Register the subscriber with the event handler
  auto registration_result =
      test_context.GetGameContext().event_handler.RegisterSubscriber(
          button_element.subscription);
  if (!registration_result) {
    FAIL("Failed to register subscriber: " +
         registration_result.error().message);
  }

  // Create response packet
  steamrot::EventPacket event_packet{steamrot::EventType_EVENT_QUIT_GAME,
                                     std::monostate()};
  button_element.response_event = event_packet;
  REQUIRE(button_element.response_event != std::nullopt);
  REQUIRE(button_element.subscription);

  // Add an unrelated triggering event to event bus: the bitset does not match
  // the subscriber
  sf::Event event_sf{sf::Event::KeyPressed()};

  steamrot::EventPacket unrelated_event_packet{steamrot::EventType_EVENT_TEST,
                                               std::monostate()};

  test_context.GetGameContext().event_handler.AddEvent(unrelated_event_packet);
  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      1);

  // Process events to notify subscribers (should not activate subscriber)
  test_context.GetGameContext()
      .event_handler.UpateSubscribersFromGlobalEventBus();

  // Check subscriber is not active (since no matching event on bus)
  REQUIRE_FALSE(button_element.subscription->IsActive());

  // Run ProcessUIActionsAndEvents; should NOT add the quit event to the event
  // bus
  steamrot::ProcessUIActionsAndEvents(
      button_element, test_context.GetGameContext().event_handler);

  // Assert - that the EventBus still has only the unrelated event (no quit
  // event added)
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      1);

  // Assert - that the only event is NOT of type EventType_EVENT_QUIT_GAME
  const auto &event =
      test_context.GetGameContext().event_handler.GetGlobalEventBus().back();
  REQUIRE(steamrot::EnumNameEventType(event.m_event_type) !=
          steamrot::EnumNameEventType(steamrot::EventType_EVENT_QUIT_GAME));
}
TEST_CASE("UIActionLogic checks subscription before adding Event to EventBus "
          "for ButtonElement"
          "[UIActionLogic]") {

  // arrange the UIActionLogic
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  steamrot::UIActionLogic ui_action_logic(
      test_context.GetLogicContextForTestScene());
  // assert that the EventBus has no events
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      0);
  // arrange a button element
  steamrot::ButtonElement button_element;
  ;
  button_element.position = {100.0f, 100.0f};
  button_element.size = {200.0f, 50.0f};
  button_element.is_mouse_over = true; // simulate mouse over
  button_element.label = "Quit Game";
  steamrot::Subscriber subscriber{steamrot::EventType_EVENT_USER_INPUT};

  button_element.subscription =
      std::make_shared<steamrot::Subscriber>(subscriber);
  // register the subscriber with the event handler
  auto registration_result =
      test_context.GetGameContext().event_handler.RegisterSubscriber(
          button_element.subscription);
  if (!registration_result) {
    FAIL("Failed to register subscriber: " +
         registration_result.error().message);
  }
  // create response packet
  steamrot::EventPacket event_packet{steamrot::EventType_EVENT_QUIT_GAME,
                                     std::monostate()};
  button_element.response_event = event_packet;
  REQUIRE(button_element.response_event != std::nullopt);
  REQUIRE(button_element.subscription);

  // add triggering event to event bus
  steamrot::EventPacket trigger_event_packet{
      steamrot::EventType_EVENT_USER_INPUT, steamrot::UserInputBitset{}};
  test_context.GetGameContext().event_handler.AddEvent(trigger_event_packet);
  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();

  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      1);

  // process events to notify subscribers
  test_context.GetGameContext()
      .event_handler.UpateSubscribersFromGlobalEventBus();

  // check subscriber is now active
  REQUIRE(button_element.subscription->IsActive());

  // run ProcessUIActionsAndEvents and this should add the quit event to the
  // event bus
  steamrot::ProcessUIActionsAndEvents(
      button_element, test_context.GetGameContext().event_handler);

  // move events from waiting room to global event bus
  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  // assert - that the EventBus has two events now
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      2);

  // assert - that the last event is of type EventType_EVENT_QUIT_GAME
  const auto &event =
      test_context.GetGameContext().event_handler.GetGlobalEventBus().back();
  REQUIRE(steamrot::EnumNameEventType(event.m_event_type) ==
          steamrot::EnumNameEventType(steamrot::EventType_EVENT_QUIT_GAME));
}

// Check that ProcessUIActionsAndEvents return Subscriber inactive
TEST_CASE("UIActionLogic sets Subscriber to inactive after processing "
          "[UIActionLogic]") {
  // arrange the UIActionLogic
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  steamrot::UIActionLogic ui_action_logic(
      test_context.GetLogicContextForTestScene());
  // assert that the EventBus has no events
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      0);
  // arrange a button element
  steamrot::ButtonElement button_element;
  ;
  button_element.position = {100.0f, 100.0f};
  button_element.size = {200.0f, 50.0f};
  button_element.is_mouse_over = true; // simulate mouse over
  button_element.label = "Quit Game";
  steamrot::Subscriber subscriber{steamrot::EventType_EVENT_USER_INPUT};
  button_element.subscription =
      std::make_shared<steamrot::Subscriber>(subscriber);
  // register the subscriber with the event handler
  auto registration_result =
      test_context.GetGameContext().event_handler.RegisterSubscriber(
          button_element.subscription);
  if (!registration_result) {
    FAIL("Failed to register subscriber: " +
         registration_result.error().message);
  }
  // create response packet
  steamrot::EventPacket event_packet{steamrot::EventType_EVENT_QUIT_GAME,
                                     std::monostate()};
  button_element.response_event = event_packet;
  REQUIRE(button_element.response_event != std::nullopt);
  REQUIRE(button_element.subscription);
  // add triggering event to event bus
  steamrot::EventPacket trigger_event_packet{
      steamrot::EventType_EVENT_USER_INPUT, steamrot::UserInputBitset{}};
  test_context.GetGameContext().event_handler.AddEvent(trigger_event_packet);
  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      1);
  // process events to notify subscribers
  test_context.GetGameContext()
      .event_handler.UpateSubscribersFromGlobalEventBus();
  // check subscriber is now active
  REQUIRE(button_element.subscription->IsActive());
  // run ProcessUIActionsAndEvents and this should add the quit event to the
  // event bus
  steamrot::ProcessUIActionsAndEvents(
      button_element, test_context.GetGameContext().event_handler);

  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  // assert - that the EventBus has two events now
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      2);

  // assert - that the last event is of type EventType_EVENT_QUIT_GAME
  const auto &event =
      test_context.GetGameContext().event_handler.GetGlobalEventBus().back();
  REQUIRE(steamrot::EnumNameEventType(event.m_event_type) ==
          steamrot::EnumNameEventType(steamrot::EventType_EVENT_QUIT_GAME));

  // now check that the subscriber is set to inactive after processing
  REQUIRE_FALSE(button_element.subscription->IsActive());
}

TEST_CASE("ProcessNestedUIActionsAndEvents processes child before parent "
          "[UIActionLogic][Nested]") {
  // Arrange
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;

  // Create a parent button element with an active subscription
  auto parent_button = std::make_unique<steamrot::ButtonElement>();
  parent_button->position = {0.0f, 0.0f};
  parent_button->size = {300.0f, 300.0f};
  parent_button->is_mouse_over = true;
  parent_button->label = "Parent Button";
  
  steamrot::Subscriber parent_subscriber{steamrot::EventType_EVENT_USER_INPUT};
  parent_button->subscription =
      std::make_shared<steamrot::Subscriber>(parent_subscriber);
  
  // Register parent subscriber
  auto parent_reg_result =
      test_context.GetGameContext().event_handler.RegisterSubscriber(
          parent_button->subscription);
  REQUIRE(parent_reg_result);
  
  steamrot::EventPacket parent_event{steamrot::EventType_EVENT_TEST,
                                     std::monostate()};
  parent_button->response_event = parent_event;

  // Create a child button element with an active subscription
  auto child_button = std::make_unique<steamrot::ButtonElement>();
  child_button->position = {50.0f, 50.0f};
  child_button->size = {200.0f, 50.0f};
  child_button->is_mouse_over = true;
  child_button->label = "Child Button";
  
  steamrot::Subscriber child_subscriber{steamrot::EventType_EVENT_USER_INPUT};
  child_button->subscription =
      std::make_shared<steamrot::Subscriber>(child_subscriber);
  
  // Register child subscriber
  auto child_reg_result =
      test_context.GetGameContext().event_handler.RegisterSubscriber(
          child_button->subscription);
  REQUIRE(child_reg_result);
  
  steamrot::EventPacket child_event{steamrot::EventType_EVENT_QUIT_GAME,
                                    std::monostate()};
  child_button->response_event = child_event;

  // Add child to parent
  parent_button->child_elements.push_back(std::move(child_button));

  // Add triggering event to activate both subscriptions
  steamrot::EventPacket trigger_event{steamrot::EventType_EVENT_USER_INPUT,
                                      steamrot::UserInputBitset{}};
  test_context.GetGameContext().event_handler.AddEvent(trigger_event);
  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  
  // Activate subscribers
  test_context.GetGameContext()
      .event_handler.UpateSubscribersFromGlobalEventBus();
  
  // Both subscriptions should be active
  REQUIRE(parent_button->subscription->IsActive());
  REQUIRE(parent_button->child_elements[0]->subscription->IsActive());

  // Act - Process nested UI actions
  steamrot::ProcessNestedUIActionsAndEvents(
      *parent_button, test_context.GetGameContext().event_handler);

  // Assert - Child subscription should be inactive (was processed)
  REQUIRE_FALSE(parent_button->child_elements[0]->subscription->IsActive());
  
  // Parent subscription should still be active (was NOT processed)
  REQUIRE(parent_button->subscription->IsActive());

  // Process waiting room to move events to global bus
  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  
  // Only child event should be in the event bus
  // (original trigger + child event = 2 events)
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      2);
  
  // The last event should be the child's event (QUIT_GAME), not parent's (TEST)
  const auto &last_event =
      test_context.GetGameContext().event_handler.GetGlobalEventBus().back();
  REQUIRE(steamrot::EnumNameEventType(last_event.m_event_type) ==
          steamrot::EnumNameEventType(steamrot::EventType_EVENT_QUIT_GAME));
}

TEST_CASE("ProcessNestedUIActionsAndEvents processes parent when child has no "
          "active subscription "
          "[UIActionLogic][Nested]") {
  // Arrange
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;

  // Create a parent button element with an active subscription
  auto parent_button = std::make_unique<steamrot::ButtonElement>();
  parent_button->position = {0.0f, 0.0f};
  parent_button->size = {300.0f, 300.0f};
  parent_button->is_mouse_over = true;
  parent_button->label = "Parent Button";
  
  steamrot::Subscriber parent_subscriber{steamrot::EventType_EVENT_USER_INPUT};
  parent_button->subscription =
      std::make_shared<steamrot::Subscriber>(parent_subscriber);
  
  // Register parent subscriber
  auto parent_reg_result =
      test_context.GetGameContext().event_handler.RegisterSubscriber(
          parent_button->subscription);
  REQUIRE(parent_reg_result);
  
  steamrot::EventPacket parent_event{steamrot::EventType_EVENT_TEST,
                                     std::monostate()};
  parent_button->response_event = parent_event;

  // Create a child button element WITHOUT an active subscription
  auto child_button = std::make_unique<steamrot::ButtonElement>();
  child_button->position = {50.0f, 50.0f};
  child_button->size = {200.0f, 50.0f};
  child_button->is_mouse_over = true;
  child_button->label = "Child Button";
  // No subscription for child

  // Add child to parent
  parent_button->child_elements.push_back(std::move(child_button));

  // Add triggering event to activate parent subscription
  steamrot::EventPacket trigger_event{steamrot::EventType_EVENT_USER_INPUT,
                                      steamrot::UserInputBitset{}};
  test_context.GetGameContext().event_handler.AddEvent(trigger_event);
  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  
  // Activate subscribers
  test_context.GetGameContext()
      .event_handler.UpateSubscribersFromGlobalEventBus();
  
  // Parent subscription should be active
  REQUIRE(parent_button->subscription->IsActive());

  // Act - Process nested UI actions
  steamrot::ProcessNestedUIActionsAndEvents(
      *parent_button, test_context.GetGameContext().event_handler);

  // Assert - Parent subscription should be inactive (was processed)
  REQUIRE_FALSE(parent_button->subscription->IsActive());

  // Process waiting room to move events to global bus
  test_context.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  
  // Parent event should be in the event bus
  // (original trigger + parent event = 2 events)
  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      2);
  
  // The last event should be the parent's event (TEST)
  const auto &last_event =
      test_context.GetGameContext().event_handler.GetGlobalEventBus().back();
  REQUIRE(steamrot::EnumNameEventType(last_event.m_event_type) ==
          steamrot::EnumNameEventType(steamrot::EventType_EVENT_TEST));
}

