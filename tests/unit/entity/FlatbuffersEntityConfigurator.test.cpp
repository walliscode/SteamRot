/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEntityConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityConfigurator.h"
#include "ButtonElement.h"
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
