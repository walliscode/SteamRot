/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEntityConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityConfigurator.h"
#include "TestFixture.h"
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
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  REQUIRE(entity_collection != nullptr);
  REQUIRE_NOTHROW(steamrot::FlatbuffersEntityConfigurator(
      game_context.event_handler, *entity_collection));
}

/////////////////////////////////////////////////
/// Individual Component Configuration Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureComponent activates base "
          "component",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  // Get a component to configure
  auto &component = steamrot::entity::memory::GetComponent<steamrot::CMeta>(
      0, scene_context.scene_entities);
  REQUIRE(component.m_active == false);

  // Configure the component
  auto result = configurator.ConfigureComponent(component);

  REQUIRE(result.has_value());
  REQUIRE(component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCUserInterface with valid "
          "data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  REQUIRE(entity_collection != nullptr);
  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, **flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(builder.GetBufferPointer()));

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  // Get the component to configure
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          1, scene_context.scene_entities);

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
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   100);

  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          2, scene_context.scene_entities);

  // This should fail because root element is missing
  auto result = configurator.ConfigureCUserInterface(ui_component);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCGrimoireMachina with "
          "valid data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          3, scene_context.scene_entities);

  REQUIRE(grimoire_component.m_active == false);

  // Configure the component directly
  auto result = configurator.ConfigureCGrimoireMachina(grimoire_component);

  REQUIRE(result.has_value());
  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCMachinaForm with valid "
          "data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  auto &machina_form_component =
      steamrot::entity::memory::GetComponent<steamrot::CMachinaForm>(
          0, scene_context.scene_entities);

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
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   10);

  // Configure first layer - should succeed even with no entities
  auto result =
      configurator.ConfigureFirstLayerComponents(scene_context.scene_entities);

  REQUIRE(result.has_value());
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents with "
          "CUserInterface",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  // Configure first layer
  auto result =
      configurator.ConfigureFirstLayerComponents(scene_context.scene_entities);

  REQUIRE(result.has_value());

  // Verify the component was configured
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          2, scene_context.scene_entities);

  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "test_ui");
  REQUIRE(ui_component.m_visible == true);
  REQUIRE(ui_component.m_root_element != nullptr);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents with "
          "CGrimoireMachina",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  // Configure first layer
  auto result =
      configurator.ConfigureFirstLayerComponents(scene_context.scene_entities);

  REQUIRE(result.has_value());

  // Verify the component was configured
  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          3, scene_context.scene_entities);

  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents with "
          "multiple components",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  // Configure first layer
  auto result =
      configurator.ConfigureFirstLayerComponents(scene_context.scene_entities);

  REQUIRE(result.has_value());

  // Verify both components were configured
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          1, scene_context.scene_entities);
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "ui_entity");

  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          1, scene_context.scene_entities);
  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents skips "
          "entities without first-layer components",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   100);

  // Configure first layer - should succeed and skip entity without components
  auto result =
      configurator.ConfigureFirstLayerComponents(scene_context.scene_entities);

  REQUIRE(result.has_value());

  // Verify that no components were activated
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          4, scene_context.scene_entities);
  REQUIRE(ui_component.m_active == false);

  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          0, scene_context.scene_entities);
  REQUIRE(grimoire_component.m_active == false);
}

/////////////////////////////////////////////////
/// Second Layer Configuration Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureSecondLayerComponents with "
          "empty entities",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   10);

  // Configure second layer - should succeed even with no entities
  auto result =
      configurator.ConfigureSecondLayerComponents(scene_context.scene_entities);

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
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(
              scene_context.scene_entities) == 0);

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;
  const size_t expected_size = 100; // Pool size from entity_test_data.json

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Configure the pool
  auto result =
      configurator.ConfigureEntityMemoryPool(scene_context.scene_entities);

  REQUIRE(result.has_value());
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(
              scene_context.scene_entities) == expected_size);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool with "
          "first-layer components",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Configure the entire pool
  auto result =
      configurator.ConfigureEntityMemoryPool(scene_context.scene_entities);

  REQUIRE(result.has_value());

  // Verify pool was resized
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(
              scene_context.scene_entities) == 5);

  // Verify component was configured
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          1, scene_context.scene_entities);
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "test_ui");
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool with "
          "multiple first-layer components",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // Configure the entire pool
  auto result =
      configurator.ConfigureEntityMemoryPool(scene_context.scene_entities);

  REQUIRE(result.has_value());

  // Verify both components were configured
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          0, scene_context.scene_entities);
  REQUIRE(ui_component.m_active == true);
  REQUIRE(ui_component.m_name == "ui_entity");

  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          5, scene_context.scene_entities);
  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool fails "
          "with invalid data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load shared entity test data
  auto test_data = LoadEntityTestData();
  const steamrot::EntityCollectionFbs *entity_collection = test_data.entity_collection;

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // This should fail during first-layer configuration (entity 2 has no root element)
  auto result =
      configurator.ConfigureEntityMemoryPool(scene_context.scene_entities);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}
