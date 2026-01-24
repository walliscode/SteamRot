/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "FlatbuffersEntityImporter.h"
#include "FlatbuffersSceneDataProvider.h"
#include "TestFixture.h"
#include "TitleScene.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "entity_memory.h"
#include "load_scene_data.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("SceneFactory::CreateEmptyScene handles UNKNOWN SceneType",
          "[unit][SceneFactory]") {
  // Arrange
  steamrot::tests::TestFixture test_fixture;
  steamrot::SceneFactory scene_factory(test_fixture.GetGameContext());

  // Act
  auto result =
      scene_factory.CreateEmptyScene(steamrot::SceneType::SceneType_UNKNOWN);

  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::EnumValueNotHandled);
  REQUIRE(result.error().message == "SceneType is UNKNOWN in SceneFactory");
}

TEST_CASE("SceneFactory::CreateEmptyScene handles bad SceneType",
          "[unit][SceneFactory]") {
  // Arrange
  steamrot::tests::TestFixture test_fixture;
  steamrot::SceneFactory scene_factory(test_fixture.GetGameContext());
  // Act
  auto result = scene_factory.CreateEmptyScene(
      static_cast<steamrot::SceneType>(9999)); // Invalid SceneType
  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::EnumValueNotHandled);
  REQUIRE(result.error().message == "SceneType not handled in SceneFactory");
}

TEST_CASE("SceneFactory::CreateEmptyScene creates TitleScene",
          "[unit][SceneFactory]") {
  // Arrange
  steamrot::tests::TestFixture test_fixture;
  steamrot::SceneFactory scene_factory(test_fixture.GetGameContext());
  // Act
  auto result =
      scene_factory.CreateEmptyScene(steamrot::SceneType::SceneType_TITLE);
  // Assert
  REQUIRE(result.has_value());
  REQUIRE(dynamic_cast<steamrot::TitleScene *>(result.value().get()) !=
          nullptr);
}

TEST_CASE("SceneFactory::CreateEmptyScene creates CraftingScene",
          "[unit][SceneFactory]") {
  // Arrange
  steamrot::tests::TestFixture test_fixture;
  steamrot::SceneFactory scene_factory(test_fixture.GetGameContext());
  // Act
  auto result =
      scene_factory.CreateEmptyScene(steamrot::SceneType::SceneType_CRAFTING);
  // Assert
  REQUIRE(result.has_value());
  REQUIRE(dynamic_cast<steamrot::CraftingScene *>(result.value().get()) !=
          nullptr);
}

TEST_CASE("SceneFactory::CreateSceneFromSceneData creates Scene with valid "
          "SceneData",
          "[unit][SceneFactory]") {

  // set up test fixture
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  // Load test data from JSON (compiled to binary)
  auto [data_buffer, scene_data_fbs] = LoadSceneTestData();
  REQUIRE(scene_data_fbs != nullptr);

  // Verify the loaded data structure
  REQUIRE(scene_data_fbs->scene_info() != nullptr);
  REQUIRE(scene_data_fbs->scene_info()->scene_type() ==
          steamrot::SceneType::SceneType_TITLE);
  REQUIRE(scene_data_fbs->scene_resources_config()->texture_width() == 800);
  REQUIRE(scene_data_fbs->scene_resources_config()->texture_height() == 600);
  REQUIRE(scene_data_fbs->entity_collection() != nullptr);
  REQUIRE(scene_data_fbs->entity_collection()->entity_memory_pool_size() == 50);

  // Check entities in the collection
  const auto *entities = scene_data_fbs->entity_collection()->entities();
  REQUIRE(entities != nullptr);
  REQUIRE(entities->size() == 2);

  // Check first entity (UI element)
  const auto *entity_0 = entities->Get(0);
  REQUIRE(entity_0 != nullptr);
  REQUIRE(entity_0->c_user_interface() != nullptr);
  REQUIRE(entity_0->c_user_interface()->ui_name()->str() == "test_ui_panel");
  REQUIRE(entity_0->c_user_interface()->is_visible() == true);

  // Check second entity (grimoire machina)
  const auto *entity_1 = entities->Get(1);
  REQUIRE(entity_1 != nullptr);
  REQUIRE(entity_1->c_grimoire_machina() != nullptr);
  REQUIRE(entity_1->c_grimoire_machina()->fragments() != nullptr);
  REQUIRE(entity_1->c_grimoire_machina()->fragments()->size() == 2);

  // Create SceneData using the constructor that takes SceneDataFbs
  steamrot::FlatbuffersSceneDataProvider scene_data_provider(
      fixture.GetGameContext().event_handler, scene_data_fbs);

  // Create SceneData object and configure it
  steamrot::SceneData scene_data;
  auto configure_result = scene_data_provider.ConfigureSceneData(scene_data);
  if (!configure_result.has_value())
    FAIL(configure_result.error().message);

  // Check scene state before CreateSceneFromData
  steamrot::SceneFactory scene_factory(fixture.GetGameContext());

  // Verify scene_data before passing to factory
  REQUIRE(scene_data.scene_info.type == steamrot::SceneType::SceneType_TITLE);

  // Act - Create scene from data
  auto result = scene_factory.CreateSceneFromSceneData(scene_data);

  // Assert - Check scene was created successfully
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  auto &scene = result.value();
  REQUIRE(scene != nullptr);
  REQUIRE(dynamic_cast<steamrot::TitleScene *>(scene.get()) != nullptr);

  // Verify scene type matches
  REQUIRE(scene->GetSceneInfo().type == steamrot::SceneType::SceneType_TITLE);
  REQUIRE(scene->GetSceneInfo().id != uuids::uuid{});
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(
              scene->GetSceneContext().scene_entities) == 50);
}

TEST_CASE("SceneFactory provides UUID if not present in SceneData",
          "[unit][SceneFactory]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;

  steamrot::SceneFactory scene_factory(fixture.GetGameContext());

  // create SceneData with nil UUID
  steamrot::SceneData scene_data;
  scene_data.scene_info.type = steamrot::SceneType_TITLE;
  scene_data.scene_resources_config.texture_width = 800;
  scene_data.scene_resources_config.texture_height = 600;

  // Create scene from scene data
  auto result = scene_factory.CreateSceneFromSceneData(scene_data);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  auto &scene = result.value();
  REQUIRE(!scene->GetSceneInfo().id.is_nil());
  REQUIRE(scene->GetSceneInfo().type == steamrot::SceneType_TITLE);
}

TEST_CASE("SceneFactory configures the scenes logic map",
          "[unit][SceneFactory]") {

  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;

  // Create SceneData with minimal configuration
  steamrot::SceneData scene_data;
  scene_data.scene_info.type = steamrot::SceneType_TITLE;
  scene_data.scene_resources_config.texture_width = 800;
  scene_data.scene_resources_config.texture_height = 600;

  steamrot::SceneFactory scene_factory(fixture.GetGameContext());

  // Create scene from data (which configures logic map)
  auto result = scene_factory.CreateSceneFromSceneData(scene_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  auto &scene = result.value();

  // check that logic map is populated
  auto &logic_collection = scene->GetSceneResources().logic_map;
  ///// CHECKING COLLISION LOGICS /////
  auto collision_it = logic_collection.find(steamrot::LogicType::Collision);
  if (collision_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Collision LogicType");
  }
  const auto &collision_logics = collision_it->second;
  REQUIRE(collision_logics.size() == 1);
  REQUIRE(
      dynamic_cast<steamrot::UICollisionLogic *>(collision_logics[0].get()));

  ///// CHECKING ACTION LOGICS /////
  auto action_it = logic_collection.find(steamrot::LogicType::Action);
  if (action_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Action LogicType");
  }
  const auto &action_logics = action_it->second;
  REQUIRE(action_logics.size() == 1);
  REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[0].get()));

  ///// CHECKING RENDER LOGICS /////
  auto render_it = logic_collection.find(steamrot::LogicType::Render);
  if (render_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Render LogicType");
  }
  const auto &render_logics = render_it->second;
  REQUIRE(render_logics.size() == 1); // No render logics added yet
  REQUIRE(dynamic_cast<steamrot::UIRenderLogic *>(render_logics[0].get()));
}
