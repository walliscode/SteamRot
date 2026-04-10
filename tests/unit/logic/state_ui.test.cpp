/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the free functions in state_ui.cpp
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "state_ui.h"
#include "CUserInterface.h"
#include "PanelElement.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UpdateCUserInterfaceVisibilityFromCUIState does nothing if "
          "CUserIntferface not registered with CUIState",
          "[logic_ui]") {
  // Arrange
  // Set up an EntityMemoryPool with one CUserInterface entity and one
  // CUIState entity
  steamrot::EntityMemoryPool scene_entities;
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_entities, 10);
  steamrot::CUserInterface &ui_entity =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          0, scene_entities);
  ui_entity.m_name = "Test_UI";
  steamrot::CUIState &ui_state =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(
          1, scene_entities);
  // Define a UI visibility state for a specific state key
  steamrot::UIVisibilityState visibility_state;
  visibility_state.m_ui_indices_on.push_back(2);  // Index not registered
  visibility_state.m_ui_indices_off.push_back(3); // Index not registered
  // Map the state key to the visibility state
  ui_state.m_state_to_ui_visibility["Test_State"] = visibility_state;

  // Set up a subscriber for the state key
  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  ui_state.m_state_subscribers["Test_State"].push_back(subscriber);
  ui_entity.m_visible = true;

  // Act
  steamrot::logic::state::ui::UpdateCUserInterfaceVisibilityFromCUIState(
      ui_state, scene_entities);
  // Assert
  REQUIRE(ui_entity.m_visible == true); // Visibility should remain unchanged
}

TEST_CASE("UpdateCUserInterfaceVisibilityFromCUIState updates UI "
          "visibility",
          "[logic_ui]") {

  // Arrange
  // Set up an EntityMemoryPool with two CUserInterface entities and
  // one CUIState entity
  steamrot::EntityMemoryPool scene_entities;
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_entities, 10);

  steamrot::CUserInterface &ui_one =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          0, scene_entities);
  ui_one.m_name = "UI_One";

  steamrot::CUserInterface &ui_two =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          1, scene_entities);
  ui_two.m_name = "UI_Two";

  steamrot::CUIState &ui_state =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(
          2, scene_entities);

  // Define a UI visibility state for a specific state key
  steamrot::UIVisibilityState visibility_state_one;
  visibility_state_one.m_ui_indices_on.push_back(0);  // UI_One on
  visibility_state_one.m_ui_indices_off.push_back(1); // UI_Two off
  // Map the state key to the visibility state
  ui_state.m_state_to_ui_visibility["State_One"] = visibility_state_one;
  // Set up subscribers for the state key
  auto subscriber_one = std::make_shared<steamrot::Subscriber>();
  ui_state.m_state_subscribers["State_One"].push_back(subscriber_one);

  // define another UI visibility state for a different state key
  steamrot::UIVisibilityState visibility_state_two;
  visibility_state_two.m_ui_indices_on.push_back(1);  // UI_Two on
  visibility_state_two.m_ui_indices_off.push_back(0); // UI_One off
  // Map the state key to the visibility state
  ui_state.m_state_to_ui_visibility["State_Two"] = visibility_state_two;
  // Set up subscribers for the second state key
  auto subscriber_two = std::make_shared<steamrot::Subscriber>();
  ui_state.m_state_subscribers["State_Two"].push_back(subscriber_two);

  // define another UI visibility state for a different state key (both on)
  steamrot::UIVisibilityState visibility_state_three;
  visibility_state_three.m_ui_indices_on.push_back(0); // UI_One on
  visibility_state_three.m_ui_indices_on.push_back(1); // UI_Two on
  // Map the state key to the visibility state
  ui_state.m_state_to_ui_visibility["State_Three"] = visibility_state_three;
  // Set up subscribers for the third state key
  auto subscriber_three = std::make_shared<steamrot::Subscriber>();
  auto subscriber_four = std::make_shared<steamrot::Subscriber>();
  ui_state.m_state_subscribers["State_Three"].push_back(subscriber_three);
  ui_state.m_state_subscribers["State_Three"].push_back(subscriber_four);

  REQUIRE(ui_state.m_state_subscribers.size() == 3);
  REQUIRE(ui_state.m_state_subscribers["State_One"].size() == 1);
  REQUIRE(ui_state.m_state_subscribers["State_Two"].size() == 1);
  REQUIRE(ui_state.m_state_subscribers["State_Three"].size() == 2);

  REQUIRE(ui_one.m_visible == false);
  REQUIRE(ui_two.m_visible == false);

  SECTION("State One activates correctly") {
    // Activate subscriber for State_One
    subscriber_one->m_active = true;
    // Act
    steamrot::logic::state::ui::UpdateCUserInterfaceVisibilityFromCUIState(
        ui_state, scene_entities);
    // Assert
    REQUIRE(ui_one.m_visible == true);  // UI_One should be visible
    REQUIRE(ui_two.m_visible == false); // UI_Two should be hidden
  }

  SECTION("State Two activates correctly") {
    // Activate subscriber for State_Two
    subscriber_two->m_active = true;
    // Act
    steamrot::logic::state::ui::UpdateCUserInterfaceVisibilityFromCUIState(
        ui_state, scene_entities);
    // Assert
    REQUIRE(ui_one.m_visible == false); // UI_One should be hidden
    REQUIRE(ui_two.m_visible == true);  // UI_Two should be visible
  }

  SECTION("State Three activates correctly with multiple subscribers") {
    REQUIRE(ui_one.m_visible == false);
    REQUIRE(ui_two.m_visible == false);
    // Activate both subscribers for State_Three
    subscriber_three->m_active = true;
    subscriber_four->m_active = true;
    // Act
    steamrot::logic::state::ui::UpdateCUserInterfaceVisibilityFromCUIState(
        ui_state, scene_entities);
    // Assert
    REQUIRE(ui_one.m_visible == true); // UI_One should be visible
    REQUIRE(ui_two.m_visible == true); // UI_Two should be visible
  }
  SECTION("State Three does not activate if one subscriber inactive") {
    REQUIRE(ui_one.m_visible == false);
    REQUIRE(ui_two.m_visible == false);
    // Activate only one subscriber for State_Three
    subscriber_three->m_active = true;
    subscriber_four->m_active = false;
    // Act
    steamrot::logic::state::ui::UpdateCUserInterfaceVisibilityFromCUIState(
        ui_state, scene_entities);
    // Assert
    REQUIRE(ui_one.m_visible == false); // UI_One should remain hidden
    REQUIRE(ui_two.m_visible == false); // UI_Two should remain hidden
  }
}

// ---------------------------------------------------------------------------
// UpdateUIDisabledStateFromCUIState tests
// ---------------------------------------------------------------------------

TEST_CASE("UpdateUIDisabledStateFromCUIState disables and enables root "
          "elements based on subscriber state",
          "[logic_ui][disabled]") {

  steamrot::EntityMemoryPool scene_entities;
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_entities, 10);

  // Create two UI entities with root elements
  steamrot::CUserInterface &ui_one =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          0, scene_entities);
  ui_one.m_name = "UI_One";
  ui_one.m_root_element = std::make_unique<steamrot::PanelElement>();

  steamrot::CUserInterface &ui_two =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          1, scene_entities);
  ui_two.m_name = "UI_Two";
  ui_two.m_root_element = std::make_unique<steamrot::PanelElement>();

  // Neither should be disabled initially
  REQUIRE(ui_one.m_root_element->is_disabled == false);
  REQUIRE(ui_two.m_root_element->is_disabled == false);

  steamrot::CUIState &ui_state =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(
          2, scene_entities);

  // State_Disable: disables UI_One
  steamrot::UIDisabledState disabled_state_one;
  disabled_state_one.m_ui_indices_disabled.push_back(0);
  ui_state.m_state_to_ui_disabled["State_Disable"] = disabled_state_one;
  auto subscriber_disable = std::make_shared<steamrot::Subscriber>();
  ui_state.m_state_subscribers["State_Disable"].push_back(subscriber_disable);

  // State_Enable: enables UI_One
  steamrot::UIDisabledState disabled_state_two;
  disabled_state_two.m_ui_indices_enabled.push_back(0);
  ui_state.m_state_to_ui_disabled["State_Enable"] = disabled_state_two;
  auto subscriber_enable = std::make_shared<steamrot::Subscriber>();
  ui_state.m_state_subscribers["State_Enable"].push_back(subscriber_enable);

  // Also need visibility entries so the visibility helper does not skip the
  // subscriber reset (empty visibility states are fine)
  ui_state.m_state_to_ui_visibility["State_Disable"] =
      steamrot::UIVisibilityState{};
  ui_state.m_state_to_ui_visibility["State_Enable"] =
      steamrot::UIVisibilityState{};

  SECTION("Disabled state applies when subscriber is active") {
    subscriber_disable->m_active = true;
    steamrot::logic::state::ui::UpdateUIDisabledStateFromCUIState(ui_state,
                                                                   scene_entities);
    REQUIRE(ui_one.m_root_element->is_disabled == true);
    REQUIRE(ui_two.m_root_element->is_disabled == false);
  }

  SECTION("Enable state restores element when subscriber is active") {
    // Manually disable first
    ui_one.m_root_element->is_disabled = true;

    subscriber_enable->m_active = true;
    steamrot::logic::state::ui::UpdateUIDisabledStateFromCUIState(ui_state,
                                                                   scene_entities);
    REQUIRE(ui_one.m_root_element->is_disabled == false);
  }

  SECTION("Nothing changes when subscriber is inactive") {
    // subscriber_disable remains inactive (default)
    steamrot::logic::state::ui::UpdateUIDisabledStateFromCUIState(ui_state,
                                                                   scene_entities);
    REQUIRE(ui_one.m_root_element->is_disabled == false);
    REQUIRE(ui_two.m_root_element->is_disabled == false);
  }
}

TEST_CASE("UpdateUIDisabledStateFromCUIState does not reset subscribers "
          "(visibility helper must do that)",
          "[logic_ui][disabled]") {

  steamrot::EntityMemoryPool scene_entities;
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_entities, 10);

  steamrot::CUserInterface &ui_one =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          0, scene_entities);
  ui_one.m_name = "UI_One";
  ui_one.m_root_element = std::make_unique<steamrot::PanelElement>();

  steamrot::CUIState &ui_state =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(
          1, scene_entities);

  steamrot::UIDisabledState disabled_state;
  disabled_state.m_ui_indices_disabled.push_back(0);
  ui_state.m_state_to_ui_disabled["State_A"] = disabled_state;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  ui_state.m_state_subscribers["State_A"].push_back(subscriber);

  // visibility helper also needs a mapping for the reset to fire
  ui_state.m_state_to_ui_visibility["State_A"] = steamrot::UIVisibilityState{};

  // Call disabled helper: should apply change but NOT reset subscriber
  steamrot::logic::state::ui::UpdateUIDisabledStateFromCUIState(ui_state,
                                                                 scene_entities);
  REQUIRE(ui_one.m_root_element->is_disabled == true);
  REQUIRE(subscriber->m_active == true); // subscriber still active

  // Now call visibility helper: should reset subscriber
  steamrot::logic::state::ui::UpdateCUserInterfaceVisibilityFromCUIState(
      ui_state, scene_entities);
  REQUIRE(subscriber->m_active == false); // subscriber now reset
}
