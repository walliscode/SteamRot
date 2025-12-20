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
#include "entities_generated.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <memory>

/////////////////////////////////////////////////
/// Helper Functions
/////////////////////////////////////////////////

namespace {

/////////////////////////////////////////////////
/// @brief Load entity test data from binary file
///
/// @return Pair of unique_ptr to data buffer and EntityCollectionFbs pointer
/////////////////////////////////////////////////
std::pair<std::unique_ptr<char[]>, const steamrot::EntityCollectionFbs *>
LoadEntityTestData() {
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path = data_dir / "entity_test_data.bin";

  std::ifstream infile(bin_file_path, std::ios::binary | std::ios::in);
  if (!infile.is_open()) {
    throw std::runtime_error("Failed to open file: " + bin_file_path.string());
  }

  infile.seekg(0, std::ios::end);
  auto length = infile.tellg();
  if (length <= 0) {
    throw std::runtime_error("Empty or invalid file: " +
                             bin_file_path.string());
  }

  infile.seekg(0, std::ios::beg);
  auto data = std::make_unique<char[]>(static_cast<size_t>(length));
  infile.read(data.get(), length);
  infile.close();

  const steamrot::EntityCollectionFbs *entity_collection =
      steamrot::GetEntityCollectionFbs(data.get());

  return {std::move(data), entity_collection};
}

} // namespace

/////////////////////////////////////////////////
/// Constructor Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator constructor succeeds with valid data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  auto [data, entity_collection] = LoadEntityTestData();

  REQUIRE(entity_collection != nullptr);

  // Constructor should succeed
  REQUIRE_NOTHROW(steamrot::FlatbuffersEntityConfigurator(event_handler,
                                                          *entity_collection));
}

/////////////////////////////////////////////////
/// ConfigureEntityMemoryPool Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEntityMemoryPool resizes pool correctly",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  // Before configuration - pool should be empty
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 0);

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  auto result = configurator.ConfigureEntityMemoryPool(emp);

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
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  // Resize pool first
  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  // Get component before configuration
  steamrot::CUserInterface &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1, emp);

  // Before - should be in default state
  REQUIRE(ui_component.m_active == false);
  REQUIRE(ui_component.m_name == "Default UI");
  REQUIRE(ui_component.m_visible == false);
  REQUIRE(ui_component.m_root_element == nullptr);

  // Configure first layer
  auto result = configurator.ConfigureFirstLayerComponents(emp);

  // After - should be configured
  REQUIRE(result.has_value());
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
  REQUIRE(panel->child_elements.empty());
}

TEST_CASE("ConfigureFirstLayerComponents configures hidden UI correctly",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  steamrot::CUserInterface &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(2, emp);

  // Before
  REQUIRE(ui_component.m_active == false);
  REQUIRE(ui_component.m_visible == false);

  auto result = configurator.ConfigureFirstLayerComponents(emp);

  // After - UI should be configured but not visible
  REQUIRE(result.has_value());
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

TEST_CASE("ConfigureCUserInterface handles null ui_name gracefully",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  auto result = configurator.ConfigureFirstLayerComponents(emp);

  // Should succeed - null ui_name should be handled
  REQUIRE(result.has_value());

  // Entity 5 has no ui_name
  const auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(5, emp);
  // Should keep default name when ui_name is null
  REQUIRE(ui_component.m_name == "Default UI");
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_root_element != nullptr);
}

TEST_CASE("ConfigureCUserInterface correctly handles false visibility",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  // Entity 2 has is_visible set to false
  steamrot::CUserInterface &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(2, emp);

  auto result = configurator.ConfigureFirstLayerComponents(emp);

  REQUIRE(result.has_value());
  // Verify false value is correctly read (not skipped)
  REQUIRE(ui_component.m_visible == false);
}

TEST_CASE("ConfigureCGrimoireMachina handles null fragments/joints",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  // Entity 3 has empty fragments/joints arrays
  steamrot::CGrimoireMachina &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(3,
                                                                         emp);

  auto result = configurator.ConfigureFirstLayerComponents(emp);

  // Should succeed with empty arrays
  REQUIRE(result.has_value());
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
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  steamrot::CGrimoireMachina &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(3,
                                                                         emp);

  // Before
  REQUIRE(grimoire_component.m_active == false);

  auto result = configurator.ConfigureFirstLayerComponents(emp);

  // After
  REQUIRE(result.has_value());
  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("ConfigureFirstLayerComponents configures CGrimoireMachina with data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  steamrot::CGrimoireMachina &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(4,
                                                                         emp);

  // Before
  REQUIRE(grimoire_component.m_active == false);

  auto result = configurator.ConfigureFirstLayerComponents(emp);

  // After - component should be activated
  // Note: Current implementation doesn't populate m_all_fragments/m_all_joints
  // from FlatBuffers data, it only sets m_active
  REQUIRE(result.has_value());
  REQUIRE(grimoire_component.m_active == true);
}

/////////////////////////////////////////////////
/// ConfigureSecondLayerComponents Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureSecondLayerComponents succeeds",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  // Configure first layer first
  auto first_result = configurator.ConfigureFirstLayerComponents(emp);
  REQUIRE(first_result.has_value());

  // Configure second layer
  auto second_result = configurator.ConfigureSecondLayerComponents(emp);

  // Should succeed (currently no-op but should not fail)
  REQUIRE(second_result.has_value());
}

/////////////////////////////////////////////////
/// ConfigureComponent Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureComponent activates base component",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

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
  auto [data, entity_collection] = LoadEntityTestData();

  // Before - empty pool
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 0);

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  auto result = configurator.ConfigureEntityMemoryPool(emp);

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
