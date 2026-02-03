/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEntityConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityConfigurator.h"
#include "ButtonElement.h"
#include "CUIState.h"
#include "EventHandler.h"
#include "PanelElement.h"
#include "SpacingAndSizing.h"
#include "containers.h"
#include "entity_memory.h"
#include "load_entity_data.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

/////////////////////////////////////////////////
/// Constructor Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator constructor succeeds with valid data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  auto [data, entity_transport] = LoadEntityTestData();

  // Constructor should succeed with EventHandler only
  REQUIRE_NOTHROW(steamrot::FlatbuffersEntityConfigurator(event_handler));
}

/////////////////////////////////////////////////
/// ConfigureEntityMemoryPool Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEntityMemoryPool resizes pool correctly",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  // Before configuration - pool should be empty
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 0);

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  // After configuration - pool should be resized
  REQUIRE(result.has_value());
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 20);
}

/////////////////////////////////////////////////
/// ConfigureFirstLayerComponents - CUserInterface Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureFirstLayerComponents configures CUserInterface correctly",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  // Before configuration - pool should be empty
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 0);

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  // Configure through the public API (which calls ConfigureFirstLayerComponents
  // internally)
  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  // After configuration - pool should be resized and components configured
  REQUIRE(result.has_value());
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 20);

  // Get component after configuration
  const steamrot::CUserInterface &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1, emp);

  // Should be configured
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "simple_ui");
  REQUIRE(ui_component.m_visible == true);
  REQUIRE(ui_component.m_root_element != nullptr);

  // Verify root element details
  auto *panel =
      dynamic_cast<steamrot::PanelElement *>(ui_component.m_root_element.get());
  REQUIRE(panel != nullptr);
  REQUIRE(panel->position == sf::Vector2f(10.0f, 20.0f));
  REQUIRE(panel->size == sf::Vector2f(100.0f, 50.0f));
  REQUIRE(panel->layout == steamrot::Layout::Horizontal);
  REQUIRE(panel->spacing_strategy == steamrot::SpacingAndSizing::None);
  REQUIRE(!panel->child_elements.empty());
}

TEST_CASE("ConfigureFirstLayerComponents configures hidden UI correctly",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  // After configuration
  REQUIRE(result.has_value());

  const steamrot::CUserInterface &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(2, emp);

  // UI should be configured but not visible
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "hidden_ui");
  REQUIRE(ui_component.m_visible == false);
  REQUIRE(ui_component.m_root_element != nullptr);

  auto *panel =
      dynamic_cast<steamrot::PanelElement *>(ui_component.m_root_element.get());
  REQUIRE(panel != nullptr);
  REQUIRE(panel->layout == steamrot::Layout::Vertical);
  REQUIRE(panel->spacing_strategy == steamrot::SpacingAndSizing::Even);
}

/////////////////////////////////////////////////
/// Null Data Handling Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureCUserInterface correctly handles false visibility",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Entity 2 has is_visible set to false
  const steamrot::CUserInterface &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(2, emp);

  // Verify false value is correctly read (not skipped)
  REQUIRE(ui_component.m_visible == false);
}

TEST_CASE("ConfigureCGrimoireMachina handles null fragments/joints",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  // Should succeed with empty arrays
  REQUIRE(result.has_value());

  // Entity 3 has empty fragments/joints arrays
  const steamrot::CGrimoireMachina &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(3,
                                                                         emp);

  REQUIRE(grimoire_component.m_active == true);
}

/////////////////////////////////////////////////
/// ConfigureFirstLayerComponents - CGrimoireMachina Tests
/////////////////////////////////////////////////

TEST_CASE(
    "ConfigureFirstLayerComponents configures empty CGrimoireMachina correctly",
    "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  // After configuration
  REQUIRE(result.has_value());

  const steamrot::CGrimoireMachina &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(3,
                                                                         emp);

  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("ConfigureFirstLayerComponents configures CGrimoireMachina with data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  // After - component should be activated
  // Note: Current implementation doesn't populate m_all_fragments/m_all_joints
  // from FlatBuffers data, it only sets m_active
  REQUIRE(result.has_value());

  const steamrot::CGrimoireMachina &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(4,
                                                                         emp);

  REQUIRE(grimoire_component.m_active == true);
}

/////////////////////////////////////////////////
/// ConfigureSecondLayerComponents Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureSecondLayerComponents succeeds",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  // ConfigureEntityMemoryPoolFromSource calls both first and second layer
  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  // Should succeed (second layer is currently no-op but should not fail)
  REQUIRE(result.has_value());
}

/////////////////////////////////////////////////
/// ConfigureSecondLayerComponents - CUIState Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureSecondLayerComponents configures CUIState basic component",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Entity 7 has CUIState with "main_menu" state
  const steamrot::CUIState &ui_state_component =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(7, emp);

  // Should be configured and activated
  REQUIRE(ui_state_component.m_active == true);
  REQUIRE(ui_state_component.m_state_to_ui_visibility.size() == 1);
  REQUIRE(ui_state_component.m_state_to_ui_visibility.contains("main_menu"));
}

TEST_CASE(
    "ConfigureSecondLayerComponents configures CUIState with ui_names_on",
    "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Entity 7 has CUIState with ui_names_on: ["simple_ui", "menu_ui"]
  const steamrot::CUIState &ui_state_component =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(7, emp);

  REQUIRE(ui_state_component.m_state_to_ui_visibility.contains("main_menu"));
  const auto &visibility_state =
      ui_state_component.m_state_to_ui_visibility.at("main_menu");

  // simple_ui is at entity 1, menu_ui is at entity 5
  REQUIRE(visibility_state.m_ui_indices_on.size() == 2);
  REQUIRE(std::find(visibility_state.m_ui_indices_on.begin(),
                    visibility_state.m_ui_indices_on.end(),
                    1) != visibility_state.m_ui_indices_on.end());
  REQUIRE(std::find(visibility_state.m_ui_indices_on.begin(),
                    visibility_state.m_ui_indices_on.end(),
                    5) != visibility_state.m_ui_indices_on.end());
}

TEST_CASE(
    "ConfigureSecondLayerComponents configures CUIState with ui_names_off",
    "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Entity 7 has CUIState with ui_names_off: ["hidden_ui"]
  const steamrot::CUIState &ui_state_component =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(7, emp);

  REQUIRE(ui_state_component.m_state_to_ui_visibility.contains("main_menu"));
  const auto &visibility_state =
      ui_state_component.m_state_to_ui_visibility.at("main_menu");

  // hidden_ui is at entity 2
  REQUIRE(visibility_state.m_ui_indices_off.size() == 1);
  REQUIRE(visibility_state.m_ui_indices_off[0] == 2);
}

TEST_CASE("ConfigureSecondLayerComponents configures CUIState with both "
          "ui_names_on and off",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Entity 8 has CUIState with both on and off lists
  const steamrot::CUIState &ui_state_component =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(8, emp);

  REQUIRE(ui_state_component.m_state_to_ui_visibility.contains(
      "settings_screen"));
  const auto &visibility_state =
      ui_state_component.m_state_to_ui_visibility.at("settings_screen");

  // ui_names_on: ["settings_ui"] - entity 6
  REQUIRE(visibility_state.m_ui_indices_on.size() == 1);
  REQUIRE(visibility_state.m_ui_indices_on[0] == 6);

  // ui_names_off: ["simple_ui", "menu_ui"] - entities 1 and 5
  REQUIRE(visibility_state.m_ui_indices_off.size() == 2);
  REQUIRE(std::find(visibility_state.m_ui_indices_off.begin(),
                    visibility_state.m_ui_indices_off.end(),
                    1) != visibility_state.m_ui_indices_off.end());
  REQUIRE(std::find(visibility_state.m_ui_indices_off.begin(),
                    visibility_state.m_ui_indices_off.end(),
                    5) != visibility_state.m_ui_indices_off.end());
}

TEST_CASE("ConfigureSecondLayerComponents configures CUIState with subscribers",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Entity 9 has CUIState with a subscriber
  const steamrot::CUIState &ui_state_component =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(9, emp);

  REQUIRE(ui_state_component.m_state_to_ui_visibility.contains("game_running"));
  REQUIRE(ui_state_component.m_state_subscribers.contains("game_running"));

  const auto &subscribers =
      ui_state_component.m_state_subscribers.at("game_running");

  REQUIRE(subscribers.size() == 1);
  REQUIRE(subscribers[0] != nullptr);
  REQUIRE(subscribers[0]->m_active == true);
}

TEST_CASE("ConfigureSecondLayerComponents configures CUIState with multiple "
          "state mappings",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Entity 10 has CUIState with multiple state keys
  const steamrot::CUIState &ui_state_component =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(10, emp);

  // Should have two state mappings
  REQUIRE(ui_state_component.m_state_to_ui_visibility.size() == 2);
  REQUIRE(ui_state_component.m_state_to_ui_visibility.contains(
      "multiple_states_a"));
  REQUIRE(ui_state_component.m_state_to_ui_visibility.contains(
      "multiple_states_b"));

  // Check first state mapping
  const auto &state_a =
      ui_state_component.m_state_to_ui_visibility.at("multiple_states_a");
  REQUIRE(state_a.m_ui_indices_on.size() == 1);
  REQUIRE(state_a.m_ui_indices_on[0] == 1); // simple_ui
  REQUIRE(state_a.m_ui_indices_off.size() == 1);
  REQUIRE(state_a.m_ui_indices_off[0] == 5); // menu_ui

  // Check second state mapping
  const auto &state_b =
      ui_state_component.m_state_to_ui_visibility.at("multiple_states_b");
  REQUIRE(state_b.m_ui_indices_on.size() == 2); // menu_ui and settings_ui
  REQUIRE(std::find(state_b.m_ui_indices_on.begin(),
                    state_b.m_ui_indices_on.end(),
                    5) != state_b.m_ui_indices_on.end());
  REQUIRE(std::find(state_b.m_ui_indices_on.begin(),
                    state_b.m_ui_indices_on.end(),
                    6) != state_b.m_ui_indices_on.end());
  REQUIRE(state_b.m_ui_indices_off.size() == 2); // simple_ui and hidden_ui
  REQUIRE(std::find(state_b.m_ui_indices_off.begin(),
                    state_b.m_ui_indices_off.end(),
                    1) != state_b.m_ui_indices_off.end());
  REQUIRE(std::find(state_b.m_ui_indices_off.begin(),
                    state_b.m_ui_indices_off.end(),
                    2) != state_b.m_ui_indices_off.end());
}

/////////////////////////////////////////////////
/// Second Layer Configuration Behavior Tests
/////////////////////////////////////////////////

TEST_CASE("Second layer configuration correctly references first layer "
          "CUserInterface entities",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Verify that first layer UI components exist and are configured
  const steamrot::CUserInterface &simple_ui =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1, emp);
  REQUIRE(simple_ui.m_active == true);
  REQUIRE(simple_ui.m_name == "simple_ui");

  const steamrot::CUserInterface &menu_ui =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(5, emp);
  REQUIRE(menu_ui.m_active == true);
  REQUIRE(menu_ui.m_name == "menu_ui");

  // Verify that second layer CUIState correctly references these UI entities
  const steamrot::CUIState &ui_state =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(7, emp);
  REQUIRE(ui_state.m_active == true);

  const auto &visibility_state =
      ui_state.m_state_to_ui_visibility.at("main_menu");

  // Verify indices match the configured UI entities
  REQUIRE(std::find(visibility_state.m_ui_indices_on.begin(),
                    visibility_state.m_ui_indices_on.end(),
                    1) != visibility_state.m_ui_indices_on.end());
  REQUIRE(std::find(visibility_state.m_ui_indices_on.begin(),
                    visibility_state.m_ui_indices_on.end(),
                    5) != visibility_state.m_ui_indices_on.end());
}

TEST_CASE(
    "Multiple CUIState entities in second layer are all configured correctly",
    "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Verify all CUIState entities are configured
  const steamrot::CUIState &ui_state_7 =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(7, emp);
  REQUIRE(ui_state_7.m_active == true);
  REQUIRE(ui_state_7.m_state_to_ui_visibility.size() == 1);

  const steamrot::CUIState &ui_state_8 =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(8, emp);
  REQUIRE(ui_state_8.m_active == true);
  REQUIRE(ui_state_8.m_state_to_ui_visibility.size() == 1);

  const steamrot::CUIState &ui_state_9 =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(9, emp);
  REQUIRE(ui_state_9.m_active == true);
  REQUIRE(ui_state_9.m_state_to_ui_visibility.size() == 1);
  REQUIRE(ui_state_9.m_state_subscribers.size() == 1);

  const steamrot::CUIState &ui_state_10 =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(10, emp);
  REQUIRE(ui_state_10.m_active == true);
  REQUIRE(ui_state_10.m_state_to_ui_visibility.size() == 2);
}

/////////////////////////////////////////////////
/// ConfigureCUIState Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureCUIState activates component and configures state mappings",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  // Configure through ConfigureEntityMemoryPoolFromSource to set up internal
  // state, but focus testing on CUIState configuration behavior
  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Test entity 7: ConfigureCUIState should have:
  // 1. Activated the component
  // 2. Created state_to_ui_visibility mapping for "main_menu"
  // 3. Resolved UI names to entity indices
  const steamrot::CUIState &ui_state_7 =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(7, emp);

  // Verify component activation
  REQUIRE(ui_state_7.m_active == true);

  // Verify state mapping was created
  REQUIRE(ui_state_7.m_state_to_ui_visibility.size() == 1);
  REQUIRE(ui_state_7.m_state_to_ui_visibility.contains("main_menu"));

  // Verify UI name resolution to indices
  const auto &visibility = ui_state_7.m_state_to_ui_visibility.at("main_menu");
  REQUIRE(visibility.m_ui_indices_on.size() == 2);
  REQUIRE(visibility.m_ui_indices_off.size() == 1);
}

TEST_CASE("ConfigureCUIState handles subscribers correctly",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  // Configure to test ConfigureCUIState subscriber handling
  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Test entity 9: ConfigureCUIState should have:
  // 1. Created subscriber from FlatBuffers data
  // 2. Registered subscriber with EventHandler
  // 3. Stored subscriber in m_state_subscribers
  const steamrot::CUIState &ui_state_9 =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(9, emp);

  REQUIRE(ui_state_9.m_active == true);
  REQUIRE(ui_state_9.m_state_subscribers.contains("game_running"));

  const auto &subscribers = ui_state_9.m_state_subscribers.at("game_running");
  REQUIRE(subscribers.size() == 1);
  REQUIRE(subscribers[0] != nullptr);
  REQUIRE(subscribers[0]->m_active == true);
}

TEST_CASE("ConfigureCUIState handles multiple states in single component",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  // Configure to test ConfigureCUIState multi-state handling
  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  REQUIRE(result.has_value());

  // Test entity 10: ConfigureCUIState should handle multiple ui_states entries
  const steamrot::CUIState &ui_state_10 =
      steamrot::entity::memory::GetComponent<steamrot::CUIState>(10, emp);

  REQUIRE(ui_state_10.m_active == true);

  // Verify both states were configured
  REQUIRE(ui_state_10.m_state_to_ui_visibility.size() == 2);
  REQUIRE(ui_state_10.m_state_to_ui_visibility.contains("multiple_states_a"));
  REQUIRE(ui_state_10.m_state_to_ui_visibility.contains("multiple_states_b"));

  // Verify each state has correct mappings
  const auto &state_a =
      ui_state_10.m_state_to_ui_visibility.at("multiple_states_a");
  REQUIRE(!state_a.m_ui_indices_on.empty());
  REQUIRE(!state_a.m_ui_indices_off.empty());

  const auto &state_b =
      ui_state_10.m_state_to_ui_visibility.at("multiple_states_b");
  REQUIRE(state_b.m_ui_indices_on.size() == 2);
  REQUIRE(state_b.m_ui_indices_off.size() == 2);
}

/////////////////////////////////////////////////
/// ConfigureComponent Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureComponent activates base component",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  auto [data, entity_transport] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  steamrot::CUserInterface component;

  // Before
  REQUIRE(component.m_active == false);

  auto result = configurator.ConfigureComponent(component);

  // After
  REQUIRE(result.has_value());
  REQUIRE(component.m_active == true);
}

/////////////////////////////////////////////////
/// Integration Tests
/////////////////////////////////////////////////

TEST_CASE("Full configuration flow works correctly",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_transport] = LoadEntityTestData();

  // Before - empty pool
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 0);

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_transport);

  // After - fully configured
  REQUIRE(result.has_value());
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 20);

  // Verify entity 0 is unconfigured (no components)
  const auto &meta_zero =
      steamrot::entity::memory::GetComponent<steamrot::CMeta>(0, emp);
  REQUIRE(meta_zero.m_active == false);

  // Verify entity 1 (CUserInterface) is configured
  const auto &ui_one =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1, emp);
  REQUIRE(ui_one.m_name == "simple_ui");
  REQUIRE(ui_one.m_visible == true);
  REQUIRE(ui_one.m_root_element != nullptr);
  // check that root element is PanelElement
  REQUIRE(dynamic_cast<steamrot::PanelElement *>(ui_one.m_root_element.get()));

  const steamrot::PanelElement &root_panel =
      static_cast<const steamrot::PanelElement &>(*ui_one.m_root_element);

  REQUIRE(root_panel.size.x == 100.0f);
  REQUIRE(root_panel.size.y == 50.0f);
  REQUIRE(root_panel.position.x == 10.0f);
  REQUIRE(root_panel.position.y == 20.0f);
  REQUIRE(root_panel.layout == steamrot::Layout::Horizontal);
  REQUIRE(root_panel.spacing_strategy == steamrot::SpacingAndSizing::None);
  REQUIRE(root_panel.child_elements.size() == 1);

  // check thet child element is ButtonElement and assign it
  REQUIRE(dynamic_cast<steamrot::ButtonElement *>(
      root_panel.child_elements[0].get()));
  const steamrot::ButtonElement &child_button =
      static_cast<const steamrot::ButtonElement &>(
          *root_panel.child_elements[0]);

  REQUIRE(child_button.size == sf::Vector2f(0.f, 0.f));
  REQUIRE(child_button.position == sf::Vector2f(0.f, 0.f));
  REQUIRE(child_button.label == "Crafting Table");
  REQUIRE(child_button.subscription != nullptr);
  const steamrot::Subscriber &button_sub = *child_button.subscription;

  REQUIRE(button_sub.m_trigger_event_data.has_value());

  /////
}
