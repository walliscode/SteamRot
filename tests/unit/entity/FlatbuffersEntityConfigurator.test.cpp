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
#include "grimoire_machina_generated.h"
#include "types_generated.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

/////////////////////////////////////////////////
/// Helper Functions
/////////////////////////////////////////////////

namespace {

////////////////////////////////////////////////////////////
/// @brief Storage for loaded binary data to keep it alive
////////////////////////////////////////////////////////////
struct EntityCollectionData {
  std::unique_ptr<char[]> binary_data;
  const steamrot::EntityCollectionFbs *entity_collection;

  EntityCollectionData(std::unique_ptr<char[]> data,
                       const steamrot::EntityCollectionFbs *collection)
      : binary_data(std::move(data)), entity_collection(collection) {}
};

////////////////////////////////////////////////////////////
/// @brief Load entity collection from binary file
///
/// @param filename Name of the .bin file (without path) in the data/ directory
/// @return EntityCollectionData containing the loaded collection
////////////////////////////////////////////////////////////
EntityCollectionData LoadEntityCollectionFromFile(const std::string &filename) {
  // Get the path to the data directory adjacent to this test file
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path = data_dir / filename;

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
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(data.get());

  if (!entity_collection) {
    throw std::runtime_error("Failed to parse EntityCollectionFbs from file: " +
                             bin_file_path.string());
  }

  return EntityCollectionData(std::move(data), entity_collection);
}

////////////////////////////////////////////////////////////
/// @brief Get a fresh EntityCollectionFbs pointer from a data file
///
/// This function loads the binary file each time it's called, ensuring
/// each test gets a fresh, independent EntityCollectionFbs object.
///
/// @param filename Name of the .bin file (e.g., "entity_collection_empty.bin")
/// @return Pointer to EntityCollectionFbs (lifetime managed by returned struct)
////////////////////////////////////////////////////////////
EntityCollectionData GetFreshEntityCollection(const std::string &filename) {
  return LoadEntityCollectionFromFile(filename);
}

} // namespace

/////////////////////////////////////////////////
/// Constructor Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::Constructor",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_empty.bin");

  REQUIRE(collection_data.entity_collection != nullptr);
  REQUIRE_NOTHROW(steamrot::FlatbuffersEntityConfigurator(
      game_context.event_handler, *collection_data.entity_collection));
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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_empty.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_ui_basic.bin");

  REQUIRE(collection_data.entity_collection != nullptr);
  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  // Get the component to configure
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          0, scene_context.scene_entities);

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

  // Load entity collection from file (without root element)
  auto collection_data = GetFreshEntityCollection("entity_collection_ui_no_root.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          0, scene_context.scene_entities);

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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_grimoire_basic.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  auto &grimoire_component =
      steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(
          0, scene_context.scene_entities);

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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_empty.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_empty.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_ui_basic.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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
          0, scene_context.scene_entities);

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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_grimoire_basic.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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
          0, scene_context.scene_entities);

  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents with "
          "multiple components",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_multi_component.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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
          0, scene_context.scene_entities);
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

  // Load entity collection from file (entity with no components)
  auto collection_data = GetFreshEntityCollection("entity_collection_no_components.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

  // Resize pool
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  // Configure first layer - should succeed and skip entity without components
  auto result =
      configurator.ConfigureFirstLayerComponents(scene_context.scene_entities);

  REQUIRE(result.has_value());

  // Verify that no components were activated
  auto &ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          0, scene_context.scene_entities);
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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_empty.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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

  // Load entity collection from file
  const size_t expected_size = 25;
  auto collection_data = GetFreshEntityCollection("entity_collection_size_25.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_ui_basic.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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
          0, scene_context.scene_entities);
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

  // Load entity collection from file
  auto collection_data = GetFreshEntityCollection("entity_collection_multi_component.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

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
          1, scene_context.scene_entities);
  REQUIRE(grimoire_component.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool fails "
          "with invalid data",
          "[unit][FlatbuffersEntityConfigurator]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Load entity collection from file (without root element - invalid)
  auto collection_data = GetFreshEntityCollection("entity_collection_ui_no_root.bin");

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *collection_data.entity_collection);

  // This should fail during first-layer configuration
  auto result =
      configurator.ConfigureEntityMemoryPool(scene_context.scene_entities);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}
