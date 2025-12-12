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
#include "types_generated.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

/////////////////////////////////////////////////
/// @brief Test FlatbuffersEntityConfigurator constructor
/////////////////////////////////////////////////
TEST_CASE("FlatbuffersEntityConfigurator constructor",
          "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Create minimal entity collection data
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(10);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

  // Get the pointer to the created EntityCollection
  const steamrot::EntityCollectionFbs *entity_collection =
      flatbuffers::GetRoot<steamrot::EntityCollectionFbs>(
          builder.GetBufferPointer());

  REQUIRE(entity_collection != nullptr);

  // Create the configurator - should not throw
  REQUIRE_NOTHROW(steamrot::FlatbuffersEntityConfigurator(
      game_context.event_handler, *entity_collection));
}

////////////////////////////////////////////////////
////// @brief Test ConfigureEntityMemoryPool resizes pool correctly
////////////////////////////////////////////////////
TEST_CASE(
    "FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool resizes pool",
    "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(
              scene_context.scene_entities) == 0);

  // Create minimal entity collection data
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  auto entities_vec = builder.CreateVector(entities);

  const size_t expected_size = 25;
  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(expected_size);
  collection_builder.add_entities(entities_vec);
  auto collection = collection_builder.Finish();

  builder.Finish(collection);

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

////////////////////////////////////////////////////
////// @brief Test ConfigureComponent sets component active
////////////////////////////////////////////////////
TEST_CASE(
    "FlatbuffersEntityConfigurator::ConfigureComponent activates component",
    "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Create minimal entity collection data
  flatbuffers::FlatBufferBuilder builder;
  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
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

  // Get a component to configure
  auto &component = steamrot::entity::memory::GetComponent<steamrot::CMeta>(
      0, scene_context.scene_entities);
  REQUIRE(component.m_active == false); // Should be inactive by default

  auto &component_two =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(
          1, scene_context.scene_entities);
  REQUIRE(component_two.m_active == false); // Should be inactive by default

  // Configure the component
  auto result = configurator.ConfigureComponent(component);

  REQUIRE(result.has_value());
  REQUIRE(component.m_active == true);

  // check that the second component is still inactive
  REQUIRE(component_two.m_active == false);
}

/////////////////////////////////////////////////
/// @brief Test ConfigureCUserInterface without root element fails
/////////////////////////////////////////////////
TEST_CASE(
    "FlatbuffersEntityConfigurator::ConfigureCUserInterface fails without "
    "root element",
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

  // Resize pool and configure
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  // This should fail because root element is missing
  auto result =
      configurator.ConfigureEntityMemoryPool(scene_context.scene_entities);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

/////////////////////////////////////////////////
/// @brief Test ConfigureCUserInterface with valid root element succeeds
/////////////////////////////////////////////////
TEST_CASE(
    "FlatbuffersEntityConfigurator::ConfigureCUserInterface succeeds with "
    "root element",
    "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Create valid UserInterface data with root element
  flatbuffers::FlatBufferBuilder builder;

  // Create root UI element (Panel)
  steamrot::Vector2fDataBuilder position_builder(builder);
  position_builder.add_x(100.0f);
  position_builder.add_y(200.0f);
  auto position = position_builder.Finish();

  steamrot::Vector2fDataBuilder size_builder(builder);
  size_builder.add_x(300.0f);
  size_builder.add_y(400.0f);
  auto size = size_builder.Finish();

  std::vector<flatbuffers::Offset<steamrot::child>> children;
  auto children_vec = builder.CreateVector(children);

  steamrot::UIElementDataBuilder base_data_builder(builder);
  base_data_builder.add_position(position);
  base_data_builder.add_size(size);
  base_data_builder.add_children_active(false);
  base_data_builder.add_children(children_vec);
  base_data_builder.add_layout(steamrot::LayoutType::LayoutType_Horizontal);
  base_data_builder.add_spacing_strategy(
      steamrot::SpacingAndSizingType::SpacingAndSizingType_None);
  auto base_data = base_data_builder.Finish();

  steamrot::PanelDataBuilder panel_builder(builder);
  panel_builder.add_base_data(base_data);
  auto panel_data = panel_builder.Finish();

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

  // Resize pool and configure

  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

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

/////////////////////////////////////////////////
/// @brief Test ConfigureCGrimoireMachina with invalid fragments fails
/////////////////////////////////////////////////
TEST_CASE("FlatbuffersEntityConfigurator::ConfigureCGrimoireMachina fails
          with "
          "invalid fragments",
          "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Create GrimoireMachina data with invalid fragment name
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<flatbuffers::String>> fragment_names;
  fragment_names.push_back(builder.CreateString("nonexistent_fragment"));
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

  // Resize pool and configure
  steamrot::entity::memory::ResizeEntityMemoryPool(scene_context.scene_entities,
                                                   5);

  // This should fail because fragment doesn't exist
  auto result =
      configurator.ConfigureFirstLayerComponents(scene_context.scene_entities);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

/////////////////////////////////////////////////
/// @brief Test ConfigureFirstLayerComponents with empty entities
/////////////////////////////////////////////////
TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents with
          "
          "empty entities",
          "[unit][FlatbuffersEntityConfigurator]") {

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  auto &game_context = fixture.GetGameContext();
  auto &scene_context = fixture.GetSceneContext();

  // Create entity collection with no entities
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  auto entities_vec = builder.CreateVector(entities);

  steamrot::EntityCollectionFbsBuilder collection_builder(builder);
  collection_builder.add_entity_memory_pool_size(10);
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
                                                   10);

  // Configure first layer - should succeed even with no entities
  auto result =
      configurator.ConfigureFirstLayerComponents(scene_context.scene_entities);

  REQUIRE(result.has_value());
}

/////////////////////////////////////////////////
/// @brief Test ConfigureFirstLayerComponents skips entities without
components
/////////////////////////////////////////////////
TEST_CASE("FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents
          skips "
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

  // Configure first layer - should succeed and skip entity without
  components auto result =
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
