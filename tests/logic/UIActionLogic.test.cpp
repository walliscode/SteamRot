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

  REQUIRE(button_element.subscription != std::nullopt);
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
          button_element.subscription.value());
  if (!registration_result) {
    FAIL("Failed to register subscriber: " +
         registration_result.error().message);
  }

  // Create response packet
  steamrot::EventPacket event_packet{steamrot::EventType_EVENT_QUIT_GAME,
                                     std::monostate()};
  button_element.response_event = event_packet;
  REQUIRE(button_element.response_event != std::nullopt);
  REQUIRE(button_element.subscription != std::nullopt);

  // Add an unrelated triggering event to event bus: the bitset does not match
  // the subscriber
  sf::Event event_sf{sf::Event::KeyPressed()};

  steamrot::EventPacket unrelated_event_packet{steamrot::EventType_EVENT_TEST,
                                               std::monostate()};

  test_context.GetGameContext().event_handler.AddToGlobalEventBus(
      {unrelated_event_packet});

  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      1);

  // Process events to notify subscribers (should not activate subscriber)
  test_context.GetGameContext()
      .event_handler.UpateSubscribersFromGlobalEventBus();

  // Check subscriber is not active (since no matching event on bus)
  REQUIRE_FALSE(button_element.subscription.value()->IsActive());

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
          button_element.subscription.value());
  if (!registration_result) {
    FAIL("Failed to register subscriber: " +
         registration_result.error().message);
  }
  // create response packet
  steamrot::EventPacket event_packet{steamrot::EventType_EVENT_QUIT_GAME,
                                     std::monostate()};
  button_element.response_event = event_packet;
  REQUIRE(button_element.response_event != std::nullopt);
  REQUIRE(button_element.subscription != std::nullopt);

  // add triggering event to event bus
  steamrot::EventPacket trigger_event_packet{
      steamrot::EventType_EVENT_USER_INPUT, steamrot::UserInputBitset{}};
  test_context.GetGameContext().event_handler.AddToGlobalEventBus(
      {trigger_event_packet});

  REQUIRE(
      test_context.GetGameContext().event_handler.GetGlobalEventBus().size() ==
      1);

  // process events to notify subscribers
  test_context.GetGameContext()
      .event_handler.UpateSubscribersFromGlobalEventBus();

  // check subscriber is now active
  REQUIRE(button_element.subscription.value()->IsActive());

  // run ProcessUIActionsAndEvents and this should add the quit event to the
  // event bus
  steamrot::ProcessUIActionsAndEvents(
      button_element, test_context.GetGameContext().event_handler);

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
