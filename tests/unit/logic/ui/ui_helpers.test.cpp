/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the free functions in ui_helpers.cpp
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ui/ui_helpers.h"
#include "CGrimoireMachina.h"
#include "CUserInterface.h"
#include "Fragment.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetAllFragmentNames returns correct fragment names", "[unit][ui_helpers]") {

  // Arrange
  steamrot::Fragment fragment1;
  fragment1.name = "FragmentA";
  steamrot::Fragment fragment2;
  fragment2.name = "FragmentB";

  steamrot::CGrimoireMachina grimoire_machina;
  grimoire_machina.m_all_fragments[fragment1.name] = fragment1;
  grimoire_machina.m_all_fragments[fragment2.name] = fragment2;

  // Act
  auto fragment_names =
      steamrot::logic::ui::GetAllFragmentNames(grimoire_machina);
  // Assert
  REQUIRE(fragment_names.size() == 2);
  std::vector<std::string> expected_names = {"FragmentA", "FragmentB"};
  REQUIRE(fragment_names == expected_names);
}

TEST_CASE("GetAllJointNames returns correct joint names", "[unit][ui_helpers]") {
  // Arrange
  steamrot::CGrimoireMachina grimoire_machina;
  grimoire_machina.m_all_joints["JointA"] = {}; // Placeholder joint
  grimoire_machina.m_all_joints["JointB"] = {}; // Placeholder joint
  // Act
  auto joint_names = steamrot::logic::ui::GetAllJointNames(grimoire_machina);
  // Assert
  REQUIRE(joint_names.size() == 2);
  std::vector<std::string> expected_names = {"JointA", "JointB"};
  REQUIRE(joint_names == expected_names);
}

TEST_CASE("UpdateCUserInterfaceVisibilityFromCUIState does nothing if "
          "CUserIntferface not registered with CUIState",
          "[unit][ui_helpers]") {
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
  steamrot::logic::ui::UpdateCUserInterfaceVisibilityFromCUIState(
      ui_state, scene_entities);
  // Assert
  REQUIRE(ui_entity.m_visible == true); // Visibility should remain unchanged
}

TEST_CASE("UpdateCUserInterfaceVisibilityFromCUIState updates UI "
          "visibility",
          "[unit][ui_helpers]") {

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
    steamrot::logic::ui::UpdateCUserInterfaceVisibilityFromCUIState(
        ui_state, scene_entities);
    // Assert
    REQUIRE(ui_one.m_visible == true);  // UI_One should be visible
    REQUIRE(ui_two.m_visible == false); // UI_Two should be hidden
  }

  SECTION("State Two activates correctly") {
    // Activate subscriber for State_Two
    subscriber_two->m_active = true;
    // Act
    steamrot::logic::ui::UpdateCUserInterfaceVisibilityFromCUIState(
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
    steamrot::logic::ui::UpdateCUserInterfaceVisibilityFromCUIState(
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
    steamrot::logic::ui::UpdateCUserInterfaceVisibilityFromCUIState(
        ui_state, scene_entities);
    // Assert
    REQUIRE(ui_one.m_visible == false); // UI_One should remain hidden
    REQUIRE(ui_two.m_visible == false); // UI_Two should remain hidden
  }
}
