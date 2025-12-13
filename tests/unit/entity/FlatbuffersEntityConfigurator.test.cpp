/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEntityConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityConfigurator.h"
#include "EventHandler.h"
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

/// @brief Load EntityCollectionFbs from the binary test data file
/// @return Pair of unique_ptr to data buffer and pointer to EntityCollectionFbs
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

/// @brief Find entity data by index in the entity collection
/// @param entity_collection Pointer to EntityCollectionFbs
/// @param index Entity index to find
/// @return Pointer to EntityDataFbs or nullptr if not found
const steamrot::EntityDataFbs *
FindEntityByIndex(const steamrot::EntityCollectionFbs *entity_collection,
                  uint32_t index) {
  if (!entity_collection || !entity_collection->entities()) {
    return nullptr;
  }

  for (const auto &entity_data : *entity_collection->entities()) {
    if (entity_data->index() == index) {
      return entity_data;
    }
  }
  return nullptr;
}

/////////////////////////////////////////////////
/// Constructor Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::Constructor",
          "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::EventHandler event_handler;
  auto [data, entity_collection] = LoadEntityTestData();

  REQUIRE(entity_collection != nullptr);

  REQUIRE_NOTHROW(steamrot::FlatbuffersEntityConfigurator(
      event_handler, *entity_collection));
}

/////////////////////////////////////////////////
/// Individual Component Configuration Tests
/// These tests focus on each component type's configuration
/// by calling the layer configuration and verifying specific components
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureComponent",
          "[unit][FlatbuffersEntityConfigurator][ConfigureComponent]") {

  steamrot::EventHandler event_handler;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  SECTION("Activates component") {
    steamrot::Component component;
    REQUIRE(component.m_active == false);

    auto result = configurator.ConfigureComponent(component);
    REQUIRE(result.has_value());
    REQUIRE(component.m_active == true);
  }
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCUserInterface",
          "[unit][FlatbuffersEntityConfigurator][ConfigureCUserInterface]") {

  SECTION("Entity at index 1 with ui_name='test_ui'") {
    steamrot::EventHandler event_handler;
    steamrot::EntityMemoryPool emp;
    auto [data, entity_collection] = LoadEntityTestData();

    REQUIRE(entity_collection != nullptr);

    // Verify entity data exists
    const steamrot::EntityDataFbs *entity_data =
        FindEntityByIndex(entity_collection, 1);
    REQUIRE(entity_data != nullptr);
    REQUIRE(entity_data->c_user_interface() != nullptr);

    steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                         *entity_collection);

    // Resize pool to match test data
    steamrot::entity::memory::ResizeEntityMemoryPool(
        emp, entity_collection->entity_memory_pool_size());

    // Get the component to configure
    auto &ui_component =
        steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1,
                                                                         emp);

    // Verify initial state
    REQUIRE(ui_component.m_active == false);
    REQUIRE(ui_component.m_name.empty());
    REQUIRE(ui_component.m_visible == false);
    REQUIRE(ui_component.m_root_element == nullptr);

    // Configure components through first layer (sets m_current_entity_data)
    auto result = configurator.ConfigureFirstLayerComponents(emp);
    REQUIRE(result.has_value());

    // Verify CUserInterface at index 1 was configured correctly
    REQUIRE(ui_component.m_active == true);
    REQUIRE(ui_component.m_name == "test_ui");
    REQUIRE(ui_component.m_visible == true);
    REQUIRE(ui_component.m_root_element != nullptr);
  }

  SECTION("Entity at index 4 with ui_name='ui_entity'") {
    steamrot::EventHandler event_handler;
    steamrot::EntityMemoryPool emp;
    auto [data, entity_collection] = LoadEntityTestData();

    // Verify entity data exists
    const steamrot::EntityDataFbs *entity_data =
        FindEntityByIndex(entity_collection, 4);
    REQUIRE(entity_data != nullptr);
    REQUIRE(entity_data->c_user_interface() != nullptr);

    steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                         *entity_collection);

    // Resize pool to match test data
    steamrot::entity::memory::ResizeEntityMemoryPool(
        emp, entity_collection->entity_memory_pool_size());

    // Get the component to configure
    auto &ui_component =
        steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(4,
                                                                         emp);

    // Verify initial state
    REQUIRE(ui_component.m_active == false);

    // Configure all first layer components
    auto result = configurator.ConfigureFirstLayerComponents(emp);
    REQUIRE(result.has_value());

    // Verify CUserInterface at index 4 was configured correctly
    REQUIRE(ui_component.m_active == true);
    REQUIRE(ui_component.m_name == "ui_entity");
    REQUIRE(ui_component.m_visible == true);
    REQUIRE(ui_component.m_root_element != nullptr);
  }
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCGrimoireMachina",
          "[unit][FlatbuffersEntityConfigurator][ConfigureCGrimoireMachina]") {

  SECTION("Entity at index 3") {
    steamrot::EventHandler event_handler;
    steamrot::EntityMemoryPool emp;
    auto [data, entity_collection] = LoadEntityTestData();

    // Verify entity data exists
    const steamrot::EntityDataFbs *entity_data =
        FindEntityByIndex(entity_collection, 3);
    REQUIRE(entity_data != nullptr);
    REQUIRE(entity_data->c_grimoire_machina() != nullptr);

    steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                         *entity_collection);

    // Resize pool to match test data
    steamrot::entity::memory::ResizeEntityMemoryPool(
        emp, entity_collection->entity_memory_pool_size());

    // Get the component to configure
    auto &grimoire_component =
        steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(3,
                                                                           emp);

    // Verify initial state
    REQUIRE(grimoire_component.m_active == false);

    // Configure components through first layer
    auto result = configurator.ConfigureFirstLayerComponents(emp);
    REQUIRE(result.has_value());

    // Verify CGrimoireMachina at index 3 was configured correctly
    REQUIRE(grimoire_component.m_active == true);
  }

  SECTION("Entity at index 5") {
    steamrot::EventHandler event_handler;
    steamrot::EntityMemoryPool emp;
    auto [data, entity_collection] = LoadEntityTestData();

    // Verify entity data exists
    const steamrot::EntityDataFbs *entity_data =
        FindEntityByIndex(entity_collection, 5);
    REQUIRE(entity_data != nullptr);
    REQUIRE(entity_data->c_grimoire_machina() != nullptr);

    steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                         *entity_collection);

    // Resize pool to match test data
    steamrot::entity::memory::ResizeEntityMemoryPool(
        emp, entity_collection->entity_memory_pool_size());

    // Get the component to configure
    auto &grimoire_component =
        steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(5,
                                                                           emp);

    // Verify initial state
    REQUIRE(grimoire_component.m_active == false);

    // Configure components through first layer
    auto result = configurator.ConfigureFirstLayerComponents(emp);
    REQUIRE(result.has_value());

    // Verify CGrimoireMachina at index 5 was configured correctly
    REQUIRE(grimoire_component.m_active == true);
  }
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCMachinaForm",
          "[unit][FlatbuffersEntityConfigurator][ConfigureCMachinaForm]") {

  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  // Resize pool to match test data
  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  SECTION("ConfigureCMachinaForm executes without error") {
    // Currently CMachinaForm has no data in test file
    // Test that function can be called safely
    auto &machina_form_component =
        steamrot::entity::memory::GetComponent<steamrot::CMachinaForm>(0, emp);

    auto result = configurator.ConfigureCMachinaForm(machina_form_component);
    REQUIRE(result.has_value());
  }
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCUIState",
          "[unit][FlatbuffersEntityConfigurator][ConfigureCUIState]") {

  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  // Resize pool to match test data
  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  SECTION("ConfigureCUIState executes without error") {
    // Currently CUIState has no data in test file
    // Test that function can be called safely
    auto &ui_state_component =
        steamrot::entity::memory::GetComponent<steamrot::CUIState>(0, emp);

    auto result = configurator.ConfigureCUIState(ui_state_component, emp);
    REQUIRE(result.has_value());
  }
}

/////////////////////////////////////////////////
/// Layer Configuration Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents",
          "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  REQUIRE(entity_collection != nullptr);
  REQUIRE(entity_collection->entities() != nullptr);

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  // Resize pool to match test data
  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  SECTION("Configures all first layer components from EntityCollectionData") {
    auto result = configurator.ConfigureFirstLayerComponents(emp);
    REQUIRE(result.has_value());

    // Verify CUserInterface components were configured (entities 1 and 4)
    auto &ui_component_1 =
        steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1,
                                                                         emp);
    REQUIRE(ui_component_1.m_active == true);
    REQUIRE(ui_component_1.m_name == "test_ui");
    REQUIRE(ui_component_1.m_root_element != nullptr);

    auto &ui_component_4 =
        steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(4,
                                                                         emp);
    REQUIRE(ui_component_4.m_active == true);
    REQUIRE(ui_component_4.m_name == "ui_entity");
    REQUIRE(ui_component_4.m_root_element != nullptr);

    // Verify CGrimoireMachina components were configured (entities 3 and 5)
    auto &grimoire_component_3 =
        steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(3,
                                                                           emp);
    REQUIRE(grimoire_component_3.m_active == true);

    auto &grimoire_component_5 =
        steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(5,
                                                                           emp);
    REQUIRE(grimoire_component_5.m_active == true);

    // Verify entities without component data remain unconfigured (entities 0
    // and 2)
    auto &ui_component_0 =
        steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(0,
                                                                         emp);
    REQUIRE(ui_component_0.m_active == false);

    auto &ui_component_2 =
        steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(2,
                                                                         emp);
    REQUIRE(ui_component_2.m_active == false);
  }
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureSecondLayerComponents",
          "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  // Resize pool to match test data
  steamrot::entity::memory::ResizeEntityMemoryPool(
      emp, entity_collection->entity_memory_pool_size());

  SECTION(
      "ConfigureSecondLayerComponents executes without error when applicable") {
    // Currently no second layer components in test data, but verify function
    // works
    auto result = configurator.ConfigureSecondLayerComponents(emp);
    REQUIRE(result.has_value());
  }

  SECTION("ConfigureSecondLayerComponents can be called after "
          "ConfigureFirstLayerComponents") {
    // This tests the typical usage pattern
    auto first_result = configurator.ConfigureFirstLayerComponents(emp);
    REQUIRE(first_result.has_value());

    auto second_result = configurator.ConfigureSecondLayerComponents(emp);
    REQUIRE(second_result.has_value());
  }
}

/////////////////////////////////////////////////
/// Full Configuration Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool",
          "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_collection] = LoadEntityTestData();

  REQUIRE(entity_collection != nullptr);
  REQUIRE(entity_collection->entity_memory_pool_size() == 100);

  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  SECTION("Resizes EntityMemoryPool correctly") {
    REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 0);

    auto result = configurator.ConfigureEntityMemoryPool(emp);
    REQUIRE(result.has_value());

    REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 100);
  }

  SECTION("Configures all components through full configuration pipeline") {
    auto result = configurator.ConfigureEntityMemoryPool(emp);
    REQUIRE(result.has_value());

    // Verify pool size
    REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 100);

    // Verify all first layer components were configured
    // CUserInterface entities
    auto &ui_component_1 =
        steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1,
                                                                         emp);
    REQUIRE(ui_component_1.m_active == true);
    REQUIRE(ui_component_1.m_name == "test_ui");
    REQUIRE(ui_component_1.m_visible == true);
    REQUIRE(ui_component_1.m_root_element != nullptr);

    auto &ui_component_4 =
        steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(4,
                                                                         emp);
    REQUIRE(ui_component_4.m_active == true);
    REQUIRE(ui_component_4.m_name == "ui_entity");
    REQUIRE(ui_component_4.m_visible == true);
    REQUIRE(ui_component_4.m_root_element != nullptr);

    // CGrimoireMachina entities
    auto &grimoire_component_3 =
        steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(3,
                                                                           emp);
    REQUIRE(grimoire_component_3.m_active == true);

    auto &grimoire_component_5 =
        steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(5,
                                                                           emp);
    REQUIRE(grimoire_component_5.m_active == true);

    // Verify unconfigured entities remain inactive
    auto &ui_component_0 =
        steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(0,
                                                                         emp);
    REQUIRE(ui_component_0.m_active == false);
  }
}
