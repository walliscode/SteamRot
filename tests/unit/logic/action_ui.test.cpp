/////////////////////////////////////////////////
/// @file
/// @brief Units tests for logic_actions free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_ui.h"
#include "DataPopulationFunctions.h"
#include "DropDownButtonElement.h"
#include "DropDownItemElement.h"
#include "EventPacket.h"
#include "EventPayload.h"
#include "EventType.h"
#include "PanelElement.h"
#include "Subscriber.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE(
    "logic::ui::action::ProcessButtonElementActions responds to various cases",
    "[logic][action][ProcessButtonElementActions]") {

  // set up
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;
  // set up a button element
  steamrot::ButtonElement button;
  button.subscription = std::make_shared<steamrot::Subscriber>();
  steamrot::EventPacket event_packet;
  event_packet.type = steamrot::EventType::USER_INPUT;
  event_packet.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
  button.response_events.push_back(event_packet);

  // initial tests
  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);

  SECTION("Button not moused over does not trigger event") {
    button.is_mouse_over = false;
    steamrot::logic::action::ui::ProcessButtonElementActions(button,
                                                             event_handler);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  }

  SECTION("Button moused over triggers event") {
    button.is_mouse_over = true;
    steamrot::logic::action::ui::ProcessButtonElementActions(button,
                                                             event_handler);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
  }
}

TEST_CASE("logic::ui::action::ProcessDropDownContainerElementActions responds "
          "to various cases",
          "[logic][action][ProcessDropDownContainerElementActions]") {

  // set up DropDownContainerElement with correct children
  steamrot::DropDownContainerElement dropdown_container;
  dropdown_container.subscription = std::make_shared<steamrot::Subscriber>();
  REQUIRE(dropdown_container.child_elements.size() == 0);
  REQUIRE_FALSE(dropdown_container.is_expanded);

  auto dropdown_list = std::make_unique<steamrot::DropDownListElement>();
  REQUIRE_FALSE(dropdown_list->is_expanded);

  auto dropdown_button = std::make_unique<steamrot::DropDownButtonElement>();
  REQUIRE_FALSE(dropdown_button->is_expanded);

  dropdown_container.child_elements.push_back(std::move(dropdown_list));
  dropdown_container.child_elements.push_back(std::move(dropdown_button));

  auto dropdown_list_ptr = dynamic_cast<steamrot::DropDownListElement *>(
      dropdown_container.child_elements[0].get());
  REQUIRE(dropdown_list_ptr != nullptr);
  auto dropdown_button_ptr = dynamic_cast<steamrot::DropDownButtonElement *>(
      dropdown_container.child_elements[1].get());
  REQUIRE(dropdown_button_ptr != nullptr);

  REQUIRE(dropdown_container.child_elements.size() == 2);

  SECTION(
      "ProcessDropDownContainerElementActions does not change is_expanded") {
    steamrot::tests::TestFixture fixture;
    steamrot::SceneContext &scene_context = fixture.GetSceneContext();
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE_FALSE(dropdown_container.is_expanded);
  }

  SECTION("ProcessDropDownContainerElmentActions is_expanded is false if just "
          "moused over") {

    dropdown_container.is_mouse_over = true;
    steamrot::tests::TestFixture fixture;
    steamrot::SceneContext &scene_context = fixture.GetSceneContext();
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE_FALSE(dropdown_container.is_expanded);
  }

  SECTION("ProcessDropDownContainerElementActions is_expanded is false if "
          "subscription active") {

    dropdown_container.subscription->m_active = true;
    steamrot::tests::TestFixture fixture;
    steamrot::SceneContext &scene_context = fixture.GetSceneContext();
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE_FALSE(dropdown_container.is_expanded);
  }

  SECTION("ProcessDropDownContainerElementActions is_expanded is false if "
          "subscription active and moused over") {
    dropdown_container.is_mouse_over = true;
    dropdown_container.subscription->m_active = true;
    steamrot::tests::TestFixture fixture;
    steamrot::SceneContext &scene_context = fixture.GetSceneContext();
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE_FALSE(dropdown_container.is_expanded);
  }

  SECTION("ProcessDropDownContainerElement (and DropDownListElement) "
          "is_expanded is true if "
          "DropDownButtonElement child[1] is_expanded is true") {
    dropdown_button_ptr->is_expanded = true;
    steamrot::tests::TestFixture fixture;
    steamrot::SceneContext &scene_context = fixture.GetSceneContext();
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE(dropdown_container.is_expanded);
    REQUIRE(dropdown_list_ptr->is_expanded);
  }

  SECTION("ProcessDropDownContainerElement (and DropDownListElement) toggles "
          "is_expanded if DropDownButtonElement is_mouse_over true and "
          "subscription active") {
    dropdown_button_ptr->is_mouse_over = true;
    dropdown_button_ptr->subscription =
        std::make_shared<steamrot::Subscriber>();
    dropdown_button_ptr->subscription->m_active = true;
    steamrot::tests::TestFixture fixture;
    steamrot::SceneContext &scene_context = fixture.GetSceneContext();
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE(dropdown_container.is_expanded);
    REQUIRE(dropdown_list_ptr->is_expanded);
    // toggles back to false on second call
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE_FALSE(dropdown_container.is_expanded);
    REQUIRE_FALSE(dropdown_list_ptr->is_expanded);
  }
}

TEST_CASE("logic::ui::action::ProcessDropDownButtonElementActions responds to "
          "various cases",
          "[logic][action][ProcessDropDownButtonElementActions]") {
  // set up
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  steamrot::DropDownButtonElement dropdown_button;
  dropdown_button.subscription = std::make_shared<steamrot::Subscriber>();
  // initial tests
  REQUIRE_FALSE(dropdown_button.is_expanded);
  REQUIRE_FALSE(dropdown_button.is_mouse_over);
  REQUIRE_FALSE(dropdown_button.subscription->m_active);

  SECTION("ProcessDropDownButtonElementActions does not change is_expanded if "
          "mouse_over false and subscription inactive") {
    steamrot::logic::action::ui::ProcessDropDownButtonElementActions(
        dropdown_button);
    REQUIRE_FALSE(dropdown_button.is_expanded);
  }

  SECTION("ProcessDropDownButtonElementActions does not change is_expanded if "
          "mouse_over true and subscription inactive") {
    dropdown_button.is_mouse_over = true;
    steamrot::logic::action::ui::ProcessDropDownButtonElementActions(
        dropdown_button);
    REQUIRE_FALSE(dropdown_button.is_expanded);
  }

  SECTION("ProcessDropDownButtonElementActions does not change is_expanded if "
          "mouse_over false and subscription active") {
    dropdown_button.subscription->m_active = true;
    steamrot::logic::action::ui::ProcessDropDownButtonElementActions(
        dropdown_button);
    REQUIRE_FALSE(dropdown_button.is_expanded);
  }

  SECTION("ProcessDropDownButtonElementActions toggles is_expanded if "
          "mouse_over true and subscription active") {
    dropdown_button.is_mouse_over = true;
    dropdown_button.subscription->m_active = true;
    steamrot::logic::action::ui::ProcessDropDownButtonElementActions(
        dropdown_button);
    REQUIRE(dropdown_button.is_expanded);

    // toggles back to false on second call
    steamrot::logic::action::ui::ProcessDropDownButtonElementActions(
        dropdown_button);
    REQUIRE_FALSE(dropdown_button.is_expanded);
  }
}

TEST_CASE("logic::ui::action::ProcessDropDownListElementActions populates "
          "dropdowns",
          "[logic][action][ProcessDropDownListElementActions]") {
  // set up
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  auto set_up_result = scene_context.asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up empty GrimoireMachina: " +
         set_up_result.error().message);
  }
  // set up a dropdown list element
  steamrot::DropDownListElement dropdown;
  REQUIRE(dropdown.child_elements.size() == 0);

  SECTION("No population when function is None") {
    steamrot::logic::action::ui::ProcessDropDownListElementActions(
        dropdown, scene_context);
    REQUIRE(dropdown.child_elements.size() == 0);
  }

  SECTION("No population when DataPopulationFunction is set but is_expanded is "
          "false") {
    dropdown.data_population_function =
        steamrot::DataPopulationFunction::GetAllFragmentNames;
    steamrot::logic::action::ui::ProcessDropDownListElementActions(
        dropdown, scene_context);
    REQUIRE(dropdown.child_elements.size() == 0);
  }

  SECTION("DropDownItemElements are added when "
          "DataPopulationFunction::GetAllFragmentNames is set and is_expanded "
          "is true") {

    // edit grimoice machina to have some fragments for testing
    auto get_grimoire_result = scene_context.asset_manager.GetGrimoireMachina();
    if (!get_grimoire_result.has_value()) {
      FAIL("Failed to get GrimoireMachina from AssetManager: " +
           get_grimoire_result.error().message);
    }
    steamrot::GrimoireMachina &grimoire_machina = *get_grimoire_result.value();

    grimoire_machina.m_all_fragments.clear();
    grimoire_machina.m_all_fragments.insert(
        {"fragment1", steamrot::Fragment{}});
    grimoire_machina.m_all_fragments.insert(
        {"fragment2", steamrot::Fragment{}});
    grimoire_machina.m_all_fragments.insert(
        {"fragment3", steamrot::Fragment{}});

    // set function
    dropdown.data_population_function =
        steamrot::DataPopulationFunction::GetAllFragmentNames;
    // set is_expanded to true
    dropdown.is_expanded = true;

    // run ProcessDropDownListElementActions, which will call the correct
    // population function based on the enum and populate the dropdown
    steamrot::logic::action::ui::ProcessDropDownListElementActions(
        dropdown, scene_context);

    REQUIRE(dropdown.child_elements.size() == 3);
    for (const auto &child : dropdown.child_elements) {
      REQUIRE(dynamic_cast<steamrot::DropDownItemElement *>(child.get()) !=
              nullptr);
    }
  }
}

TEST_CASE("logic::ui::action::ProcessUIActionsAndEvents processes UI elements "
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
  button.subscription = std::make_shared<steamrot::Subscriber>();
  steamrot::EventPacket event_packet;
  event_packet.type = steamrot::EventType::USER_INPUT;
  event_packet.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
  button.response_events.push_back(event_packet);

  // initial tests
  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);

  SECTION("Response not triggered when subscriber not active") {
    button.subscription->m_active = false;
    steamrot::logic::action::ui::ProcessUIActionsAndEvents(
        button, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  }
  SECTION("Response not triggered when is_mouse_over is false") {
    button.is_mouse_over = false;
    steamrot::logic::action::ui::ProcessUIActionsAndEvents(
        button, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  }
  SECTION("Response not triggerd when subscription is active and is_mouse_over "
          "is false") {
    button.is_mouse_over = false;
    button.subscription->m_active = true;

    steamrot::logic::action::ui::ProcessUIActionsAndEvents(
        button, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  }

  SECTION("Response triggered when subscriber active") {
    button.subscription->m_active = true;
    steamrot::logic::action::ui::ProcessUIActionsAndEvents(
        button, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
  }
}

TEST_CASE("logic::ui::action::ProcessNestedUIActionsAndEvents processes nested "
          "UI elements correctly",
          "[logic][action][ProcessNestedUIActionsAndEvents]") {
  // set up
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;
  // set up a panel with multiple button children
  steamrot::PanelElement panel;
  panel.children_active = true;
  auto button = std::make_unique<steamrot::ButtonElement>();
  button->is_mouse_over = true;
  button->subscription = std::make_shared<steamrot::Subscriber>();
  steamrot::EventPacket event_packet;
  event_packet.type = steamrot::EventType::USER_INPUT;
  event_packet.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};

  button->response_events.push_back(event_packet);
  panel.child_elements.push_back(std::move(button));

  auto button2 = std::make_unique<steamrot::ButtonElement>();
  button2->is_mouse_over = true;
  button2->subscription = std::make_shared<steamrot::Subscriber>();
  steamrot::EventPacket event_packet2;
  event_packet2.type = steamrot::EventType::USER_INPUT;
  event_packet2.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
  button2->response_events.push_back(event_packet2);
  panel.child_elements.push_back(std::move(button2));

  // initial tests
  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  SECTION("Child processed, parent not processed") {
    panel.child_elements[0]->subscription->m_active = true;
    steamrot::logic::action::ui::ProcessNestedUIActionsAndEvents(
        panel, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
  }
  SECTION("No child processed, parent processed") {
    panel.child_elements[0]->subscription->m_active = false;
    steamrot::logic::action::ui::ProcessNestedUIActionsAndEvents(
        panel, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  }
  SECTION("If first child is_mouse_over false, second child processed") {
    panel.child_elements[0]->is_mouse_over = false;
    panel.child_elements[0]->subscription->m_active = true;
    panel.child_elements[1]->subscription->m_active = true;
    steamrot::logic::action::ui::ProcessNestedUIActionsAndEvents(
        panel, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
  }
}
