/////////////////////////////////////////////////
/// @file
/// @brief Units tests for logic_actions free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "logic_action.h"
#include "EventPacket.h"
#include "EventType.h"
#include "PanelElement.h"
#include "Subscriber.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE(
    "logic::action::ProcessButtonElementActions responds to various cases",
    "[logic][action][ProcessButtonElementActions]") {

  // set up
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;
  // set up a button element
  steamrot::ButtonElement button;
  button.subscription =
      std::make_shared<steamrot::Subscriber>(steamrot::EventType::USER_INPUT);
  steamrot::EventPacket event_packet{steamrot::EventType::USER_INPUT,
                                     steamrot::UserInputBitset{}, 2};
  button.response_event = event_packet;

  // initial tests
  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);

  SECTION("Button not moused over does not trigger event") {
    button.is_mouse_over = false;
    steamrot::logic::action::ProcessButtonElementActions(button, event_handler);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  }

  SECTION("Button moused over triggers event") {
    button.is_mouse_over = true;
    steamrot::logic::action::ProcessButtonElementActions(button, event_handler);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
  }
}

TEST_CASE(
    "logic::action::ProcessDropDownListElementActions populates dropdowns",
    "[logic][action][ProcessDropDownListElementActions]") {
  // set up
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  // set up a dropdown list element
  steamrot::DropDownListElement dropdown;
  dropdown.data_populate_function =
      steamrot::DataPopulateFunction::DataPopulateFunction_None;
  SECTION("No population when function is None") {
    steamrot::logic::action::ProcessDropDownListElementActions(dropdown,
                                                               scene_context);
    REQUIRE(dropdown.child_elements.size() == 0);
  }
  // Further tests would require setting up a CGrimoireMachina entity in the
  // scene context, which is beyond the scope of this unit test.
}

TEST_CASE("logic::action::ProcessUIActionsAndEvents processes UI elements "
          "correctly",
          "[logic][action][ProcessUIActionsAndEvents]") {
  // set up
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  // set up a button with subscriber and response event
  steamrot::ButtonElement button;
  button.is_mouse_over = true;
  button.subscription =
      std::make_shared<steamrot::Subscriber>(steamrot::EventType::USER_INPUT);
  steamrot::EventPacket event_packet{steamrot::EventType::USER_INPUT,
                                     steamrot::UserInputBitset{}, 2};
  button.response_event = event_packet;

  // initial tests
  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);

  SECTION("Response not triggered when subscriber not active") {
    button.subscription->m_active = false;
    steamrot::logic::action::ProcessUIActionsAndEvents(button, event_handler,
                                                       scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  }

  SECTION("Response triggered when subscriber active") {
    button.subscription->m_active = true;
    steamrot::logic::action::ProcessUIActionsAndEvents(button, event_handler,
                                                       scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
  }
}

TEST_CASE("logic::action::ProcessNestedUIActionsAndEvents processes nested "
          "UI elements correctly",
          "[logic][action][ProcessNestedUIActionsAndEvents]") {
  // set up
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;
  // set up a panel with a button child
  steamrot::PanelElement panel;
  panel.children_active = true;
  auto button = std::make_unique<steamrot::ButtonElement>();
  button->is_mouse_over = true;
  button->subscription =
      std::make_shared<steamrot::Subscriber>(steamrot::EventType::USER_INPUT);
  steamrot::EventPacket event_packet{steamrot::EventType::USER_INPUT,
                                     steamrot::UserInputBitset{}, 2};
  button->response_event = event_packet;
  panel.child_elements.push_back(std::move(button));

  // initial tests
  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  SECTION("Child processed, parent not processed") {
    panel.child_elements[0]->subscription->m_active = true;
    steamrot::logic::action::ProcessNestedUIActionsAndEvents(
        panel, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
  }
  SECTION("No child processed, parent processed") {
    panel.child_elements[0]->subscription->m_active = false;
    steamrot::logic::action::ProcessNestedUIActionsAndEvents(
        panel, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  }
}
