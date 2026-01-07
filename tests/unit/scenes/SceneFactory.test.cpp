/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "FlatbuffersSceneLoadDataProvider.h"
#include "TestFixture.h"
#include "TitleScene.h"
#include "entity_memory.h"
#include "load_scene_data.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneFactory::CreateEmptyScene handles UNKNOWN SceneType",
          "[SceneFactory]") {
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
          "[SceneFactory]") {
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
          "[SceneFactory]") {
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
          "[SceneFactory]") {
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

TEST_CASE("SceneFactory::CreateSceneFromSceneLoadData creates Scene with valid "
          "SceneLoadData",
          "[SceneFactory]") {

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

  // Create SceneLoadData
  steamrot::FlatbuffersSceneLoadDataProvider scene_load_provider(
      fixture.GetGameContext().event_handler);
  auto get_scene_load_data_result =
      scene_load_provider.ProvideSceneLoadDataFromData(scene_data_fbs);
  if (!get_scene_load_data_result)
    FAIL(get_scene_load_data_result.error().message);

  const steamrot::SceneLoadData &scene_load_data =
      get_scene_load_data_result.value();

  // Check scene state before CreateSceneFromData
  steamrot::SceneFactory scene_factory(fixture.GetGameContext());

  // Verify scene_data before passing to factory
  REQUIRE(scene_load_data.scene_data.scene_info.type ==
          steamrot::SceneType::SceneType_TITLE);

  // Act - Create scene from data
  auto result = scene_factory.CreateSceneFromSceneLoadData(scene_load_data);

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
