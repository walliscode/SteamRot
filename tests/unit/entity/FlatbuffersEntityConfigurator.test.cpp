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
#include <iostream>

/////////////////////////////////////////////////
/// Helper Functions
/////////////////////////////////////////////////

namespace {

////////////////////////////////////////////////////////////
/// @brief Helper to create a minimal empty entity collection
////////////////////////////////////////////////////////////
flatbuffers::FlatBufferBuilder CreateEmptyCollection(size_t pool_size = 10) {
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(pool_size);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);
  return builder;
}

////////////////////////////////////////////////////////////
/// @brief Helper to create a valid Panel UIElement
////////////////////////////////////////////////////////////
flatbuffers::Offset<steamrot::PanelData>
CreateValidPanelData(flatbuffers::FlatBufferBuilder &builder) {
  // Create position
  steamrot::Vector2fDataBuilder position_builder(builder);
  position_builder.add_x(100.0f);
  position_builder.add_y(200.0f);
  auto position = position_builder.Finish();

  // Create size
  steamrot::Vector2fDataBuilder size_builder(builder);
  size_builder.add_x(300.0f);
  size_builder.add_y(400.0f);
  auto size = size_builder.Finish();

  // Create empty children vector
  std::vector<flatbuffers::Offset<steamrot::child>> children;
  auto children_vec = builder.CreateVector(children);

  // Create base UI element data
  steamrot::UIElementDataBuilder base_data_builder(builder);
  base_data_builder.add_position(position);
  base_data_builder.add_size(size);
  base_data_builder.add_children_active(false);
  base_data_builder.add_children(children_vec);
  base_data_builder.add_layout(steamrot::LayoutType::LayoutType_Horizontal);
  base_data_builder.add_spacing_strategy(
      steamrot::SpacingAndSizingType::SpacingAndSizingType_None);
  auto base_data = base_data_builder.Finish();

  // Create panel data
  steamrot::PanelDataBuilder panel_builder(builder);
  panel_builder.add_base_data(base_data);
  return panel_builder.Finish();
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

  auto builder = CreateEmptyCollection();
  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  auto builder = CreateEmptyCollection(5);
  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  // Create UserInterface data with root element
  flatbuffers::FlatBufferBuilder builder;

  auto panel_data = CreateValidPanelData(builder);
  auto ui_name = builder.CreateString("test_ui");

  std::cout << "Panel Data created." << std::endl;
  steamrot::UserInterfaceDataBuilder ui_builder(builder);
  ui_builder.add_ui_name(ui_name);
  ui_builder.add_is_visible(true);
  ui_builder.add_root_ui_element(panel_data);
  auto ui_data = ui_builder.Finish();

  std::cout << "UI Data created." << std::endl;
  // Create entity with this UI data
  steamrot::EntityDataFbsBuilder entity_builder(builder);
  entity_builder.add_index(0);
  entity_builder.add_c_user_interface(ui_data);
  auto entity = entity_builder.Finish();

  std::cout << "Entity created." << std::endl;
  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity);
  auto entities_vec = builder.CreateVector(entities);

  std::cout << "Entities vector created." << std::endl;
  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

  REQUIRE(entity_collection != nullptr);
  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

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

  // Create UserInterface data without root element
  flatbuffers::FlatBufferBuilder builder;

  auto ui_name = builder.CreateString("test_ui");

  steamrot::UserInterfaceDataBuilder ui_builder(builder);
  ui_builder.add_ui_name(ui_name);
  ui_builder.add_is_visible(true);
  // Intentionally NOT adding root_ui_element
  auto ui_data = ui_builder.Finish();

  // Create entity with this UI data
  steamrot::EntityDataFbsBuilder entity_builder(builder);
  entity_builder.add_index(0);
  entity_builder.add_c_user_interface(ui_data);
  auto entity = entity_builder.Finish();

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity);
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

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

  // Create GrimoireMachina data with empty fragments/joints (valid minimal
  // data)
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<flatbuffers::String>> fragment_names;
  auto fragments_vec = builder.CreateVector(fragment_names);

  std::vector<flatbuffers::Offset<flatbuffers::String>> joint_names;
  auto joints_vec = builder.CreateVector(joint_names);

  steamrot::GrimoireMachinaDataBuilder grimoire_builder(builder);
  grimoire_builder.add_fragments(fragments_vec);
  grimoire_builder.add_joints(joints_vec);
  auto grimoire_data = grimoire_builder.Finish();

  // Create entity with this data
  steamrot::EntityDataFbsBuilder entity_builder(builder);
  entity_builder.add_index(0);
  entity_builder.add_c_grimoire_machina(grimoire_data);
  auto entity = entity_builder.Finish();

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity);
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

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

  // Create minimal entity collection for testing
  auto builder = CreateEmptyCollection(5);
  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  auto builder = CreateEmptyCollection(10);
  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  // Create valid UserInterface data
  flatbuffers::FlatBufferBuilder builder;

  auto panel_data = CreateValidPanelData(builder);
  auto ui_name = builder.CreateString("test_ui");

  steamrot::UserInterfaceDataBuilder ui_builder(builder);
  ui_builder.add_ui_name(ui_name);
  ui_builder.add_is_visible(true);
  ui_builder.add_root_ui_element(panel_data);
  auto ui_data = ui_builder.Finish();

  // Create entity with this UI data
  steamrot::EntityDataFbsBuilder entity_builder(builder);
  entity_builder.add_index(0);
  entity_builder.add_c_user_interface(ui_data);
  auto entity = entity_builder.Finish();

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity);
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  // Create GrimoireMachina data
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<flatbuffers::String>> fragment_names;
  auto fragments_vec = builder.CreateVector(fragment_names);

  std::vector<flatbuffers::Offset<flatbuffers::String>> joint_names;
  auto joints_vec = builder.CreateVector(joint_names);

  steamrot::GrimoireMachinaDataBuilder grimoire_builder(builder);
  grimoire_builder.add_fragments(fragments_vec);
  grimoire_builder.add_joints(joints_vec);
  auto grimoire_data = grimoire_builder.Finish();

  // Create entity with this data
  steamrot::EntityDataFbsBuilder entity_builder(builder);
  entity_builder.add_index(0);
  entity_builder.add_c_grimoire_machina(grimoire_data);
  auto entity = entity_builder.Finish();

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity);
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  // Create entity collection with both CUserInterface and CGrimoireMachina
  flatbuffers::FlatBufferBuilder builder;

  // Create first entity with CUserInterface
  auto panel_data_1 = CreateValidPanelData(builder);
  auto ui_name_1 = builder.CreateString("ui_entity");

  steamrot::UserInterfaceDataBuilder ui_builder_1(builder);
  ui_builder_1.add_ui_name(ui_name_1);
  ui_builder_1.add_is_visible(true);
  ui_builder_1.add_root_ui_element(panel_data_1);
  auto ui_data_1 = ui_builder_1.Finish();

  steamrot::EntityDataFbsBuilder entity_builder_1(builder);
  entity_builder_1.add_index(0);
  entity_builder_1.add_c_user_interface(ui_data_1);
  auto entity_1 = entity_builder_1.Finish();

  // Create second entity with CGrimoireMachina
  std::vector<flatbuffers::Offset<flatbuffers::String>> fragment_names;
  auto fragments_vec = builder.CreateVector(fragment_names);

  std::vector<flatbuffers::Offset<flatbuffers::String>> joint_names;
  auto joints_vec = builder.CreateVector(joint_names);

  steamrot::GrimoireMachinaDataBuilder grimoire_builder(builder);
  grimoire_builder.add_fragments(fragments_vec);
  grimoire_builder.add_joints(joints_vec);
  auto grimoire_data = grimoire_builder.Finish();

  steamrot::EntityDataFbsBuilder entity_builder_2(builder);
  entity_builder_2.add_index(1);
  entity_builder_2.add_c_grimoire_machina(grimoire_data);
  auto entity_2 = entity_builder_2.Finish();

  // Create collection with both entities
  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity_1);
  entities.push_back(entity_2);
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  // Create entity with no first-layer components (just an index)
  flatbuffers::FlatBufferBuilder builder;

  steamrot::EntityDataFbsBuilder entity_builder(builder);
  entity_builder.add_index(0);
  // Not adding any component data
  auto entity = entity_builder.Finish();

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity);
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

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

  auto builder = CreateEmptyCollection(10);
  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  const size_t expected_size = 25;
  auto builder = CreateEmptyCollection(expected_size);
  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  // Create entity with CUserInterface
  flatbuffers::FlatBufferBuilder builder;

  auto panel_data = CreateValidPanelData(builder);
  auto ui_name = builder.CreateString("test_ui");

  steamrot::UserInterfaceDataBuilder ui_builder(builder);
  ui_builder.add_ui_name(ui_name);
  ui_builder.add_is_visible(true);
  ui_builder.add_root_ui_element(panel_data);
  auto ui_data = ui_builder.Finish();

  steamrot::EntityDataFbsBuilder entity_builder(builder);
  entity_builder.add_index(0);
  entity_builder.add_c_user_interface(ui_data);
  auto entity = entity_builder.Finish();

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity);
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  // Create entities with different first-layer components
  flatbuffers::FlatBufferBuilder builder;

  // Entity 0: CUserInterface
  auto panel_data = CreateValidPanelData(builder);
  auto ui_name = builder.CreateString("ui_entity");

  steamrot::UserInterfaceDataBuilder ui_builder(builder);
  ui_builder.add_ui_name(ui_name);
  ui_builder.add_is_visible(true);
  ui_builder.add_root_ui_element(panel_data);
  auto ui_data = ui_builder.Finish();

  steamrot::EntityDataFbsBuilder entity_builder_0(builder);
  entity_builder_0.add_index(0);
  entity_builder_0.add_c_user_interface(ui_data);
  auto entity_0 = entity_builder_0.Finish();

  // Entity 1: CGrimoireMachina
  std::vector<flatbuffers::Offset<flatbuffers::String>> fragment_names;
  auto fragments_vec = builder.CreateVector(fragment_names);

  std::vector<flatbuffers::Offset<flatbuffers::String>> joint_names;
  auto joints_vec = builder.CreateVector(joint_names);

  steamrot::GrimoireMachinaDataBuilder grimoire_builder(builder);
  grimoire_builder.add_fragments(fragments_vec);
  grimoire_builder.add_joints(joints_vec);
  auto grimoire_data = grimoire_builder.Finish();

  steamrot::EntityDataFbsBuilder entity_builder_1(builder);
  entity_builder_1.add_index(1);
  entity_builder_1.add_c_grimoire_machina(grimoire_data);
  auto entity_1 = entity_builder_1.Finish();

  // Create collection
  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity_0);
  entities.push_back(entity_1);
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

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

  // Create UserInterface data without root element (invalid)
  flatbuffers::FlatBufferBuilder builder;

  auto ui_name = builder.CreateString("test_ui");

  steamrot::UserInterfaceDataBuilder ui_builder(builder);
  ui_builder.add_ui_name(ui_name);
  ui_builder.add_is_visible(true);
  // Intentionally NOT adding root_ui_element
  auto ui_data = ui_builder.Finish();

  steamrot::EntityDataFbsBuilder entity_builder(builder);
  entity_builder.add_index(0);
  entity_builder.add_c_user_interface(ui_data);
  auto entity = entity_builder.Finish();

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  entities.push_back(entity);
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(5);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

  steamrot::FlatbuffersEntityConfigurator configurator(
      game_context.event_handler, *entity_collection);

  // This should fail during first-layer configuration
  auto result =
      configurator.ConfigureEntityMemoryPool(scene_context.scene_entities);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}
