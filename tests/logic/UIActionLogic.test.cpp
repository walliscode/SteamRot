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
  steamrot::Subscriber subscriber{steamrot::EventType_EVENT_USER_INPUT,
                                  steamrot::UserInputBitset{2}};
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
