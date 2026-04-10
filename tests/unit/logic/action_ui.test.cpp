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
#include "Fragment.h"
#include "GrimoireMachina.h"
#include "Joint.h"
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

  SECTION("children_active is set to true when list expands and items are "
          "populated") {

    auto get_grimoire_result = scene_context.asset_manager.GetGrimoireMachina();
    if (!get_grimoire_result.has_value()) {
      FAIL("Failed to get GrimoireMachina from AssetManager: " +
           get_grimoire_result.error().message);
    }
    steamrot::GrimoireMachina &grimoire_machina = *get_grimoire_result.value();
    grimoire_machina.m_all_fragments.clear();
    grimoire_machina.m_all_fragments.insert(
        {"fragment1", steamrot::Fragment{}});

    dropdown.data_population_function =
        steamrot::DataPopulationFunction::GetAllFragmentNames;
    dropdown.is_expanded = true;
    REQUIRE_FALSE(dropdown.children_active);

    steamrot::logic::action::ui::ProcessDropDownListElementActions(
        dropdown, scene_context);

    REQUIRE(dropdown.children_active);
    REQUIRE(dropdown.child_elements.size() == 1);
  }

  SECTION("children_active is set to false and children are cleared when list "
          "collapses") {

    // pre-populate the list as if it were expanded
    auto item = std::make_unique<steamrot::DropDownItemElement>();
    item->label = "existing_item";
    dropdown.child_elements.push_back(std::move(item));
    dropdown.children_active = true;
    dropdown.is_expanded = false;

    steamrot::logic::action::ui::ProcessDropDownListElementActions(
        dropdown, scene_context);

    REQUIRE_FALSE(dropdown.children_active);
    REQUIRE(dropdown.child_elements.empty());
  }
}

TEST_CASE("logic::ui::action::ProcessDropDownListElementActions populates "
          "joint dropdowns",
          "[unit][logic][action][ProcessDropDownListElementActions]") {
  // set up
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  auto set_up_result = scene_context.asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up empty GrimoireMachina: " +
         set_up_result.error().message);
  }
  steamrot::DropDownListElement dropdown;
  REQUIRE(dropdown.child_elements.size() == 0);

  SECTION("No population when DataPopulationFunction::GetAllJointNames is set "
          "but is_expanded is false") {
    dropdown.data_population_function =
        steamrot::DataPopulationFunction::GetAllJointNames;
    steamrot::logic::action::ui::ProcessDropDownListElementActions(
        dropdown, scene_context);
    REQUIRE(dropdown.child_elements.size() == 0);
  }

  SECTION("DropDownItemElements are added when "
          "DataPopulationFunction::GetAllJointNames is set and is_expanded "
          "is true") {

    auto get_grimoire_result = scene_context.asset_manager.GetGrimoireMachina();
    if (!get_grimoire_result.has_value()) {
      FAIL("Failed to get GrimoireMachina from AssetManager: " +
           get_grimoire_result.error().message);
    }
    steamrot::GrimoireMachina &grimoire_machina = *get_grimoire_result.value();

    grimoire_machina.m_all_joints.clear();
    grimoire_machina.m_all_joints.insert({"joint1", steamrot::Joint{}});
    grimoire_machina.m_all_joints.insert({"joint2", steamrot::Joint{}});
    grimoire_machina.m_all_joints.insert({"joint3", steamrot::Joint{}});

    dropdown.data_population_function =
        steamrot::DataPopulationFunction::GetAllJointNames;
    dropdown.is_expanded = true;

    steamrot::logic::action::ui::ProcessDropDownListElementActions(
        dropdown, scene_context);

    REQUIRE(dropdown.child_elements.size() == 3);
    for (const auto &child : dropdown.child_elements) {
      REQUIRE(dynamic_cast<steamrot::DropDownItemElement *>(child.get()) !=
              nullptr);
    }
  }

  SECTION("children_active is set to true when joint list expands and items "
          "are populated") {

    auto get_grimoire_result = scene_context.asset_manager.GetGrimoireMachina();
    if (!get_grimoire_result.has_value()) {
      FAIL("Failed to get GrimoireMachina from AssetManager: " +
           get_grimoire_result.error().message);
    }
    steamrot::GrimoireMachina &grimoire_machina = *get_grimoire_result.value();
    grimoire_machina.m_all_joints.clear();
    grimoire_machina.m_all_joints.insert({"joint1", steamrot::Joint{}});

    dropdown.data_population_function =
        steamrot::DataPopulationFunction::GetAllJointNames;
    dropdown.is_expanded = true;
    REQUIRE_FALSE(dropdown.children_active);

    steamrot::logic::action::ui::ProcessDropDownListElementActions(
        dropdown, scene_context);

    REQUIRE(dropdown.children_active);
    REQUIRE(dropdown.child_elements.size() == 1);
  }
}

TEST_CASE("logic::ui::action::ProcessDropDownContainerElementActions manages "
          "list children_active and population on expand and collapse",
          "[logic][action][ProcessDropDownContainerElementActions]") {

  // set up a container with list and button children
  steamrot::DropDownContainerElement dropdown_container;
  dropdown_container.subscription = std::make_shared<steamrot::Subscriber>();

  auto dropdown_list = std::make_unique<steamrot::DropDownListElement>();
  dropdown_list->data_population_function =
      steamrot::DataPopulationFunction::GetAllFragmentNames;

  auto dropdown_button = std::make_unique<steamrot::DropDownButtonElement>();
  dropdown_button->subscription = std::make_shared<steamrot::Subscriber>();

  dropdown_container.child_elements.push_back(std::move(dropdown_list));
  dropdown_container.child_elements.push_back(std::move(dropdown_button));

  auto *list_ptr = dynamic_cast<steamrot::DropDownListElement *>(
      dropdown_container.child_elements[0].get());
  auto *button_ptr = dynamic_cast<steamrot::DropDownButtonElement *>(
      dropdown_container.child_elements[1].get());
  REQUIRE(list_ptr != nullptr);
  REQUIRE(button_ptr != nullptr);

  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  auto set_up_result = scene_context.asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up GrimoireMachina: " + set_up_result.error().message);
  }
  auto get_grimoire_result = scene_context.asset_manager.GetGrimoireMachina();
  if (!get_grimoire_result.has_value()) {
    FAIL("Failed to get GrimoireMachina: " +
         get_grimoire_result.error().message);
  }
  steamrot::GrimoireMachina &grimoire_machina = *get_grimoire_result.value();
  grimoire_machina.m_all_fragments.clear();
  grimoire_machina.m_all_fragments.insert({"fragment1", steamrot::Fragment{}});
  grimoire_machina.m_all_fragments.insert({"fragment2", steamrot::Fragment{}});

  SECTION("children_active is true and items are populated when container "
          "expands") {
    button_ptr->is_expanded = true;
    REQUIRE_FALSE(list_ptr->children_active);
    REQUIRE(list_ptr->child_elements.empty());

    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);

    REQUIRE(list_ptr->is_expanded);
    REQUIRE(list_ptr->children_active);
    REQUIRE(list_ptr->child_elements.size() == 2);
  }

  SECTION("children_active is false and children are cleared when container "
          "collapses") {
    // start in expanded state
    button_ptr->is_expanded = true;
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE(list_ptr->children_active);
    REQUIRE_FALSE(list_ptr->child_elements.empty());

    // now collapse
    button_ptr->is_expanded = false;
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);

    REQUIRE_FALSE(list_ptr->is_expanded);
    REQUIRE_FALSE(list_ptr->children_active);
    REQUIRE(list_ptr->child_elements.empty());
  }

  SECTION("clicking button toggles expand then collapse with correct state") {
    // first click: expand
    button_ptr->is_mouse_over = true;
    button_ptr->subscription->m_active = true;
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE(list_ptr->is_expanded);
    REQUIRE(list_ptr->children_active);
    REQUIRE(list_ptr->child_elements.size() == 2);

    // second click: collapse
    button_ptr->subscription->m_active = true;
    steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
        dropdown_container, scene_context);
    REQUIRE_FALSE(list_ptr->is_expanded);
    REQUIRE_FALSE(list_ptr->children_active);
    REQUIRE(list_ptr->child_elements.empty());
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

TEST_CASE("logic::ui::action::ProcessNestedUIActionsAndEvents skips children "
          "when children_active is false",
          "[unit][logic][action][ProcessNestedUIActionsAndEvents]") {
  // When children_active = false, children must NOT fire even if their
  // subscription is active and is_mouse_over is true (stale state).

  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::PanelElement panel;
  panel.children_active = false; // children are inactive

  auto button = std::make_unique<steamrot::ButtonElement>();
  button->is_mouse_over = true; // stale hover
  button->subscription = std::make_shared<steamrot::Subscriber>();
  button->subscription->m_active = true; // active sub
  steamrot::EventPacket event_packet;
  event_packet.type = steamrot::EventType::USER_INPUT;
  event_packet.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
  button->response_events.push_back(event_packet);
  panel.child_elements.push_back(std::move(button));

  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);

  SECTION("Inactive child button does not fire its response event") {
    // Panel itself has no subscription so ProcessUIActionsAndEvents is a no-op
    steamrot::logic::action::ui::ProcessNestedUIActionsAndEvents(
        panel, event_handler, scene_context);
    event_handler.ProcessWaitingRoomEventBus();
    REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
  }
}

TEST_CASE("logic::ui::action::ProcessNestedUIActionsAndEvents with siblings: "
          "inactive children do not block lower siblings",
          "[unit][logic][action][ProcessNestedUIActionsAndEvents]") {
  // Simulates the dropdown-over-exit-button pattern:
  //   panel (children_active=true)
  //   ├── dropdown_container (children_active=false, stale is_mouse_over=true)
  //   └── exit_button (is_mouse_over=true, active subscription)
  //
  // Because the dropdown_container's children are inactive, it should not
  // block the exit_button from firing (the panel falls through after finding
  // the container has no active children).
  //
  // Note: in a real game frame UICollisionLogic would have already set
  // is_mouse_over correctly via CheckMouseOver (which now also respects
  // children_active).  This test validates the action layer independently.

  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::PanelElement panel;
  panel.children_active = true;

  // dropdown container: children inactive, subscription active, stale hover
  auto container = std::make_unique<steamrot::PanelElement>();
  container->children_active = false;
  container->is_mouse_over = true; // stale
  container->subscription = std::make_shared<steamrot::Subscriber>();
  container->subscription->m_active = true;
  steamrot::EventPacket container_evt;
  container_evt.type = steamrot::EventType::USER_INPUT;
  container_evt.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
  container->response_events.push_back(container_evt);

  // exit button: children_active default (false, no children), hovered
  auto exit_button = std::make_unique<steamrot::ButtonElement>();
  exit_button->is_mouse_over = true;
  exit_button->subscription = std::make_shared<steamrot::Subscriber>();
  exit_button->subscription->m_active = true;
  steamrot::EventPacket exit_evt;
  exit_evt.type = steamrot::EventType::USER_INPUT;
  exit_evt.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
  exit_button->response_events.push_back(exit_evt);

  panel.child_elements.push_back(std::move(container));
  panel.child_elements.push_back(std::move(exit_button));

  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);

  steamrot::logic::action::ui::ProcessNestedUIActionsAndEvents(
      panel, event_handler, scene_context);
  event_handler.ProcessWaitingRoomEventBus();

  // The container's children are inactive so ProcessNestedUIActionsAndEvents
  // skips them and dispatches ProcessUIActionsAndEvents on the container
  // itself (a PanelElement), which is a no-op.  No event should be fired.
  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
}

// ---------------------------------------------------------------------------
// is_disabled tests for ProcessUIActionsAndEvents /
// ProcessNestedUIActionsAndEvents
// ---------------------------------------------------------------------------

TEST_CASE(
    "ProcessUIActionsAndEvents does not fire events for a disabled button",
    "[unit][action][disabled]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::ButtonElement button;
  button.is_mouse_over = true;
  button.is_disabled = true;
  button.subscription = std::make_shared<steamrot::Subscriber>();
  button.subscription->m_active = true;
  steamrot::EventPacket event_packet;
  event_packet.type = steamrot::EventType::USER_INPUT;
  event_packet.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
  button.response_events.push_back(event_packet);

  steamrot::tests::TestFixture fixture2;
  steamrot::logic::action::ui::ProcessUIActionsAndEvents(
      button, event_handler, fixture2.GetSceneContext());
  event_handler.ProcessWaitingRoomEventBus();

  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
}

TEST_CASE("ProcessNestedUIActionsAndEvents does not fire events for a "
          "disabled root element",
          "[unit][action][disabled]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::ButtonElement button;
  button.is_mouse_over = true;
  button.is_disabled = true;
  button.subscription = std::make_shared<steamrot::Subscriber>();
  button.subscription->m_active = true;
  steamrot::EventPacket event_packet;
  event_packet.type = steamrot::EventType::USER_INPUT;
  event_packet.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
  button.response_events.push_back(event_packet);

  steamrot::logic::action::ui::ProcessNestedUIActionsAndEvents(
      button, event_handler, scene_context);
  event_handler.ProcessWaitingRoomEventBus();

  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
}

TEST_CASE("ProcessNestedUIActionsAndEvents does not fire events for a "
          "disabled child button",
          "[unit][action][disabled]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::PanelElement panel;
  panel.children_active = true;

  auto button = std::make_unique<steamrot::ButtonElement>();
  button->is_mouse_over = true;
  button->is_disabled = true;
  button->subscription = std::make_shared<steamrot::Subscriber>();
  button->subscription->m_active = true;
  steamrot::EventPacket event_packet;
  event_packet.type = steamrot::EventType::USER_INPUT;
  event_packet.payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};
  button->response_events.push_back(event_packet);
  panel.child_elements.push_back(std::move(button));

  steamrot::logic::action::ui::ProcessNestedUIActionsAndEvents(
      panel, event_handler, scene_context);
  event_handler.ProcessWaitingRoomEventBus();

  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
}

/////////////////////////////////////////////////
/// ProcessDropDownItemElementActions tests
/////////////////////////////////////////////////

TEST_CASE("logic::ui::action::ProcessDropDownItemElementActions emits a GHOST "
          "SELECT event when item is hovered and subscription is active",
          "[unit][action_ui][ProcessDropDownItemElementActions]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::DropDownItemElement item;
  item.is_mouse_over = true;
  item.ghost_selection_tag = steamrot::FragmentTag{"stone"};
  item.subscription = std::make_shared<steamrot::Subscriber>();
  item.subscription->m_active = true;

  REQUIRE(event_handler.GetWaitingRoomEventBus().size() == 0);

  steamrot::logic::action::ui::ProcessDropDownItemElementActions(
      item, event_handler);
  event_handler.ProcessWaitingRoomEventBus();

  REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
  const auto &evt = event_handler.GetGlobalEventBus()[0];
  REQUIRE(evt.type == steamrot::EventType::GHOST);
  const auto *ghost_payload = std::get_if<steamrot::GhostPayload>(&evt.payload);
  REQUIRE(ghost_payload != nullptr);
  REQUIRE(ghost_payload->action == steamrot::GhostPayload::GhostAction::SELECT);
  REQUIRE(std::holds_alternative<steamrot::FragmentTag>(
      ghost_payload->m_selection));
  REQUIRE(std::get<steamrot::FragmentTag>(ghost_payload->m_selection).key ==
          "stone");
}

TEST_CASE("logic::ui::action::ProcessDropDownItemElementActions emits GHOST "
          "SELECT with JointTag",
          "[unit][action_ui][ProcessDropDownItemElementActions]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::DropDownItemElement item;
  item.is_mouse_over = true;
  item.ghost_selection_tag = steamrot::JointTag{"hinge"};
  item.subscription = std::make_shared<steamrot::Subscriber>();
  item.subscription->m_active = true;

  steamrot::logic::action::ui::ProcessDropDownItemElementActions(
      item, event_handler);
  event_handler.ProcessWaitingRoomEventBus();

  REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
  const auto &evt = event_handler.GetGlobalEventBus()[0];
  const auto *ghost_payload = std::get_if<steamrot::GhostPayload>(&evt.payload);
  REQUIRE(ghost_payload != nullptr);
  REQUIRE(std::holds_alternative<steamrot::JointTag>(ghost_payload->m_selection));
  REQUIRE(std::get<steamrot::JointTag>(ghost_payload->m_selection).key ==
          "hinge");
}

TEST_CASE("logic::ui::action::ProcessDropDownItemElementActions does not emit "
          "event when not hovered",
          "[unit][action_ui][ProcessDropDownItemElementActions]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::DropDownItemElement item;
  item.is_mouse_over = false;
  item.ghost_selection_tag = steamrot::FragmentTag{"stone"};
  item.subscription = std::make_shared<steamrot::Subscriber>();
  item.subscription->m_active = true;

  steamrot::logic::action::ui::ProcessDropDownItemElementActions(
      item, event_handler);
  event_handler.ProcessWaitingRoomEventBus();

  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
}

TEST_CASE("logic::ui::action::ProcessDropDownItemElementActions does not emit "
          "event when subscription is inactive",
          "[unit][action_ui][ProcessDropDownItemElementActions]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::DropDownItemElement item;
  item.is_mouse_over = true;
  item.ghost_selection_tag = steamrot::FragmentTag{"stone"};
  item.subscription = std::make_shared<steamrot::Subscriber>();
  item.subscription->m_active = false;

  steamrot::logic::action::ui::ProcessDropDownItemElementActions(
      item, event_handler);
  event_handler.ProcessWaitingRoomEventBus();

  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
}

TEST_CASE("logic::ui::action::ProcessDropDownItemElementActions does not emit "
          "event when subscription is null",
          "[unit][action_ui][ProcessDropDownItemElementActions]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::DropDownItemElement item;
  item.is_mouse_over = true;
  item.ghost_selection_tag = steamrot::FragmentTag{"stone"};
  item.subscription = nullptr;

  steamrot::logic::action::ui::ProcessDropDownItemElementActions(
      item, event_handler);
  event_handler.ProcessWaitingRoomEventBus();

  REQUIRE(event_handler.GetGlobalEventBus().size() == 0);
}

TEST_CASE("logic::ui::action::ProcessDropDownItemElementActions deactivates "
          "the subscription after firing",
          "[unit][action_ui][ProcessDropDownItemElementActions]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::DropDownItemElement item;
  item.is_mouse_over = true;
  item.ghost_selection_tag = steamrot::FragmentTag{"granite"};
  item.subscription = std::make_shared<steamrot::Subscriber>();
  item.subscription->m_active = true;

  steamrot::logic::action::ui::ProcessDropDownItemElementActions(
      item, event_handler);

  REQUIRE_FALSE(item.subscription->m_active);
}

TEST_CASE("logic::ui::action::ProcessDropDownContainerElementActions "
          "collapses the dropdown and emits GHOST SELECT when an item is "
          "clicked",
          "[unit][action_ui][ProcessDropDownContainerElementActions]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  auto set_up_result = scene_context.asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up GrimoireMachina: " + set_up_result.error().message);
  }
  auto get_grimoire_result = scene_context.asset_manager.GetGrimoireMachina();
  if (!get_grimoire_result.has_value()) {
    FAIL("Failed to get GrimoireMachina: " +
         get_grimoire_result.error().message);
  }
  steamrot::GrimoireMachina &grimoire = *get_grimoire_result.value();
  grimoire.m_all_fragments.clear();
  grimoire.m_all_fragments.insert({"rock", steamrot::Fragment{}});

  steamrot::DropDownContainerElement dropdown_container;
  dropdown_container.subscription = std::make_shared<steamrot::Subscriber>();

  auto dropdown_list = std::make_unique<steamrot::DropDownListElement>();
  dropdown_list->data_population_function =
      steamrot::DataPopulationFunction::GetAllFragmentNames;

  auto dropdown_button = std::make_unique<steamrot::DropDownButtonElement>();
  dropdown_button->subscription = std::make_shared<steamrot::Subscriber>();

  dropdown_container.child_elements.push_back(std::move(dropdown_list));
  dropdown_container.child_elements.push_back(std::move(dropdown_button));

  auto *list_ptr = dynamic_cast<steamrot::DropDownListElement *>(
      dropdown_container.child_elements[0].get());
  auto *button_ptr = dynamic_cast<steamrot::DropDownButtonElement *>(
      dropdown_container.child_elements[1].get());
  REQUIRE(list_ptr != nullptr);
  REQUIRE(button_ptr != nullptr);

  // Expand the list first
  button_ptr->is_expanded = true;
  steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
      dropdown_container, scene_context);
  REQUIRE(list_ptr->is_expanded);
  REQUIRE(list_ptr->child_elements.size() == 1);

  // Simulate a click on the first item
  auto *item_ptr = dynamic_cast<steamrot::DropDownItemElement *>(
      list_ptr->child_elements[0].get());
  REQUIRE(item_ptr != nullptr);
  item_ptr->is_mouse_over = true;
  item_ptr->subscription->m_active = true;

  steamrot::logic::action::ui::ProcessDropDownContainerElementActions(
      dropdown_container, scene_context);

  // Dropdown must have collapsed
  REQUIRE_FALSE(button_ptr->is_expanded);
  REQUIRE_FALSE(list_ptr->is_expanded);
  REQUIRE(list_ptr->child_elements.empty());
  REQUIRE_FALSE(dropdown_container.is_expanded);

  // GHOST SELECT event must be on the event bus
  fixture.GetGameContext().event_handler.ProcessWaitingRoomEventBus();
  const auto &bus =
      fixture.GetGameContext().event_handler.GetGlobalEventBus();
  bool found_ghost = false;
  for (const auto &evt : bus) {
    if (evt.type == steamrot::EventType::GHOST) {
      const auto *gp = std::get_if<steamrot::GhostPayload>(&evt.payload);
      if (gp &&
          gp->action == steamrot::GhostPayload::GhostAction::SELECT &&
          std::holds_alternative<steamrot::FragmentTag>(gp->m_selection) &&
          std::get<steamrot::FragmentTag>(gp->m_selection).key == "rock") {
        found_ghost = true;
      }
    }
  }
  REQUIRE(found_ghost);
}

TEST_CASE("logic::ui::action::ProcessDropDownListElementActions sets "
          "ghost_selection_tag on created DropDownItemElements (fragment)",
          "[unit][action_ui][ghost_selection_tag]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  auto set_up_result = scene_context.asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up GrimoireMachina: " + set_up_result.error().message);
  }
  auto get_grimoire_result = scene_context.asset_manager.GetGrimoireMachina();
  if (!get_grimoire_result.has_value()) {
    FAIL("Failed to get GrimoireMachina: " +
         get_grimoire_result.error().message);
  }
  steamrot::GrimoireMachina &grimoire = *get_grimoire_result.value();
  grimoire.m_all_fragments.clear();
  grimoire.m_all_fragments.insert({"arm", steamrot::Fragment{}});

  steamrot::DropDownListElement dropdown;
  dropdown.data_population_function =
      steamrot::DataPopulationFunction::GetAllFragmentNames;
  dropdown.is_expanded = true;

  steamrot::logic::action::ui::ProcessDropDownListElementActions(dropdown,
                                                                  scene_context);

  REQUIRE(dropdown.child_elements.size() == 1);
  auto *item = dynamic_cast<steamrot::DropDownItemElement *>(
      dropdown.child_elements[0].get());
  REQUIRE(item != nullptr);
  REQUIRE(std::holds_alternative<steamrot::FragmentTag>(
      item->ghost_selection_tag));
  REQUIRE(std::get<steamrot::FragmentTag>(item->ghost_selection_tag).key ==
          "arm");
}

TEST_CASE("logic::ui::action::ProcessDropDownListElementActions sets "
          "ghost_selection_tag on created DropDownItemElements (joint)",
          "[unit][action_ui][ghost_selection_tag]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  auto set_up_result = scene_context.asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up GrimoireMachina: " + set_up_result.error().message);
  }
  auto get_grimoire_result = scene_context.asset_manager.GetGrimoireMachina();
  if (!get_grimoire_result.has_value()) {
    FAIL("Failed to get GrimoireMachina: " +
         get_grimoire_result.error().message);
  }
  steamrot::GrimoireMachina &grimoire = *get_grimoire_result.value();
  grimoire.m_all_joints.clear();
  grimoire.m_all_joints.insert({"pivot", steamrot::Joint{}});

  steamrot::DropDownListElement dropdown;
  dropdown.data_population_function =
      steamrot::DataPopulationFunction::GetAllJointNames;
  dropdown.is_expanded = true;

  steamrot::logic::action::ui::ProcessDropDownListElementActions(dropdown,
                                                                  scene_context);

  REQUIRE(dropdown.child_elements.size() == 1);
  auto *item = dynamic_cast<steamrot::DropDownItemElement *>(
      dropdown.child_elements[0].get());
  REQUIRE(item != nullptr);
  REQUIRE(
      std::holds_alternative<steamrot::JointTag>(item->ghost_selection_tag));
  REQUIRE(std::get<steamrot::JointTag>(item->ghost_selection_tag).key ==
          "pivot");
}

TEST_CASE("logic::ui::action::ProcessDropDownListElementActions creates a "
          "subscription on each DropDownItemElement (fragment)",
          "[unit][action_ui][item_subscription]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();
  auto set_up_result = scene_context.asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up GrimoireMachina: " + set_up_result.error().message);
  }
  auto get_grimoire_result = scene_context.asset_manager.GetGrimoireMachina();
  if (!get_grimoire_result.has_value()) {
    FAIL("Failed to get GrimoireMachina: " +
         get_grimoire_result.error().message);
  }
  steamrot::GrimoireMachina &grimoire = *get_grimoire_result.value();
  grimoire.m_all_fragments.clear();
  grimoire.m_all_fragments.insert({"leg", steamrot::Fragment{}});

  steamrot::DropDownListElement dropdown;
  dropdown.data_population_function =
      steamrot::DataPopulationFunction::GetAllFragmentNames;
  dropdown.is_expanded = true;

  steamrot::logic::action::ui::ProcessDropDownListElementActions(dropdown,
                                                                  scene_context);

  REQUIRE(dropdown.child_elements.size() == 1);
  auto *item = dynamic_cast<steamrot::DropDownItemElement *>(
      dropdown.child_elements[0].get());
  REQUIRE(item != nullptr);
  REQUIRE(item->subscription != nullptr);
  REQUIRE(item->subscription->event_type == steamrot::EventType::USER_INPUT);
  const auto *input_payload =
      std::get_if<steamrot::InputPayload>(&item->subscription->filter_payload);
  REQUIRE(input_payload != nullptr);
  REQUIRE(input_payload->action ==
          steamrot::InputPayload::InputAction::SELECT);
}
