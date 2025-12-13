/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEntityConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityConfigurator.h"
#include "EventHandler.h"
#include "entity_memory.h"
#include "entities_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <memory>

/////////////////////////////////////////////////
/// Helper Functions
/////////////////////////////////////////////////

namespace {

////////////////////////////////////////////////////////////
/// @brief Storage for loaded binary data and entity collection
////////////////////////////////////////////////////////////
struct EntityTestData {
  std::unique_ptr<char[]> binary_data;
  const steamrot::EntityCollectionFbs *entity_collection;

  EntityTestData(std::unique_ptr<char[]> data,
                 const steamrot::EntityCollectionFbs *collection)
      : binary_data(std::move(data)), entity_collection(collection) {}
};

////////////////////////////////////////////////////////////
/// @brief Minimal test fixture for entity configurator tests
///
/// Provides only the minimal dependencies needed for testing
/// FlatbuffersEntityConfigurator without pulling in heavy dependencies
/// like scenes, logic, display, etc.
////////////////////////////////////////////////////////////
struct MinimalTestFixture {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool entity_pool;

  MinimalTestFixture() {
    // Initialize with a reasonable default size
    steamrot::entity::memory::ResizeEntityMemoryPool(entity_pool, 10);
  }
};

////////////////////////////////////////////////////////////
/// @brief Load entity test data from binary file
///
/// @return EntityTestData containing the shared entity collection
////////////////////////////////////////////////////////////
EntityTestData LoadEntityTestData() {
  // Get the path to the data directory adjacent to this test file
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path = data_dir / "entity_test_data.bin";

  // Open and read the binary file
  std::ifstream infile(bin_file_path, std::ios::binary | std::ios::in);
  if (!infile.is_open()) {
    throw std::runtime_error("Failed to open file: " + bin_file_path.string());
  }

  infile.seekg(0, std::ios::end);
  auto length = infile.tellg();
  if (length <= 0) {
    throw std::runtime_error("Empty or invalid file: " + bin_file_path.string());
  }

  infile.seekg(0, std::ios::beg);
  auto data = std::make_unique<char[]>(static_cast<size_t>(length));
  infile.read(data.get(), length);
  infile.close();

  // Get the root and verify
  const steamrot::EntityCollectionFbs *entity_collection =
      steamrot::GetEntityCollectionFbs(data.get());

  if (!entity_collection) {
    throw std::runtime_error("Failed to parse EntityCollectionFbs from file: " +
                             bin_file_path.string());
  }

  return EntityTestData(std::move(data), entity_collection);
}

// Entity indices in the test data file:
// 0: Entity with no components
// 1: Entity with CUserInterface (with root element)
// 2: Entity with CUserInterface (without root element)
// 3: Entity with CGrimoireMachina
// 4: Entity with CUserInterface (ui_entity name)
// 5: Entity with CGrimoireMachina

} // namespace

/////////////////////////////////////////////////
/// Constructor Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::Constructor",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  REQUIRE(entity_collection != nullptr);
  REQUIRE_NOTHROW(steamrot::FlatbuffersEntityConfigurator(
      fixture.event_handler, *entity_collection));
}

/////////////////////////////////////////////////
/// Individual Component Configuration Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureComponent activates base "
          "component",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  // Get a component to configure
  auto &component = steamrot::entity::memory::GetComponent<steamrot::CMeta>(
      0, fixture.entity_pool);
  REQUIRE(component.m_active == false);

  // Configure the component
  auto result = configurator.ConfigureComponent(component);

  REQUIRE(result.has_value());
  REQUIRE(component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCUserInterface with valid "
          "data",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  REQUIRE(entity_collection != nullptr);
  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  // Get the component to configure
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          1, fixture.entity_pool);

  REQUIRE(ui_component.m_active == false);

  // Configure the component directly
  auto result = configurator.ConfigureCUserInterface(ui_component);

  REQUIRE(result.has_value());
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "test_ui");
  REQUIRE(ui_component.m_visible == true);
  REQUIRE(ui_component.m_root_element != nullptr);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCUserInterface without "
          "root element fails",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          2, fixture.entity_pool);

  // This should fail because root element is missing
  auto result = configurator.ConfigureCUserInterface(ui_component);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCGrimoireMachina with "
          "valid data",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          3, fixture.entity_pool);

  REQUIRE(grimoire_component.m_active == false);

  // Configure the component directly
  auto result = configurator.ConfigureCGrimoireMachina(grimoire_component);

  REQUIRE(result.has_value());
  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCMachinaForm with valid "
          "data",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  auto &machina_form_component =
      steamrot::entity::memory::GetComponent<steamrot::CMachinaForm>(
          0, fixture.entity_pool);

  // Configure the component directly (currently a no-op but should succeed)
  auto result = configurator.ConfigureCMachinaForm(machina_form_component);

  REQUIRE(result.has_value());
}

/////////////////////////////////////////////////
/// First Layer Configuration Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents with "
          "empty entities",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  // Configure first layer - should succeed even with no entities
  auto result =
      configurator.ConfigureFirstLayerComponents(fixture.entity_pool);

  REQUIRE(result.has_value());
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents with "
          "CUserInterface",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  // Configure first layer
  auto result =
      configurator.ConfigureFirstLayerComponents(fixture.entity_pool);

  REQUIRE(result.has_value());

  // Verify the component was configured
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          2, fixture.entity_pool);

  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "test_ui");
  REQUIRE(ui_component.m_visible == true);
  REQUIRE(ui_component.m_root_element != nullptr);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents with "
          "CGrimoireMachina",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  // Configure first layer
  auto result =
      configurator.ConfigureFirstLayerComponents(fixture.entity_pool);

  REQUIRE(result.has_value());

  // Verify the component was configured
  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          3, fixture.entity_pool);

  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents with "
          "multiple components",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  // Configure first layer
  auto result =
      configurator.ConfigureFirstLayerComponents(fixture.entity_pool);

  REQUIRE(result.has_value());

  // Verify both components were configured
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          1, fixture.entity_pool);
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "ui_entity");

  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          1, fixture.entity_pool);
  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents skips "
          "entities without first-layer components",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  // Configure first layer - should succeed and skip entity without components
  auto result =
      configurator.ConfigureFirstLayerComponents(fixture.entity_pool);

  REQUIRE(result.has_value());

  // Verify that no components were activated
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          4, fixture.entity_pool);
  REQUIRE(ui_component.m_active == false);

  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          0, fixture.entity_pool);
  REQUIRE(grimoire_component.m_active == false);
}

/////////////////////////////////////////////////
/// Second Layer Configuration Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureSecondLayerComponents with "
          "empty entities",
          "[unit][FlatbuffersEntityConfigurator]") {
  MinimalTestFixture fixture;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      fixture.event_handler, *entity_collection);

  // Configure second layer - should succeed even with no entities
  auto result =
      configurator.ConfigureSecondLayerComponents(fixture.entity_pool);

  REQUIRE(result.has_value());
}

// Note: CUIState and CMachinaForm are second-layer components
// These tests would be added when those components are fully implemented
// and require first-layer components to be configured first

/////////////////////////////////////////////////
/// Full ConfigureEntityMemoryPool Tests
/////////////////////////////////////////////////

TEST_CASE(
    "FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool resizes pool",
    "[unit][FlatbuffersEntityConfigurator]") {
  // Create fixture with size 0 initially
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool entity_pool;

  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(entity_pool) == 0);

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;
  const size_t expected_size = 100; // Pool size from entity_test_data.json

  steamrot::FlatbuffersEntityConfigurator configurator(
      event_handler, *entity_collection);

  // Configure the pool
  auto result = configurator.ConfigureEntityMemoryPool(entity_pool);

  REQUIRE(result.has_value());
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(entity_pool) == expected_size);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool with "
          "first-layer components",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool entity_pool;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      event_handler, *entity_collection);

  // Configure the entire pool
  auto result = configurator.ConfigureEntityMemoryPool(entity_pool);

  REQUIRE(result.has_value());

  // Verify pool was resized
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(entity_pool) == 5);

  // Verify component was configured
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          1, entity_pool);
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "test_ui");
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool with "
          "multiple first-layer components",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool entity_pool;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      event_handler, *entity_collection);

  // Configure the entire pool
  auto result = configurator.ConfigureEntityMemoryPool(entity_pool);

  REQUIRE(result.has_value());

  // Verify both components were configured
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          0, entity_pool);
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "ui_entity");

  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          5, entity_pool);
  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool fails "
          "with invalid data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool entity_pool;

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      event_handler, *entity_collection);

  // This should fail during first-layer configuration (entity 2 has no root element)
  auto result = configurator.ConfigureEntityMemoryPool(entity_pool);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}
