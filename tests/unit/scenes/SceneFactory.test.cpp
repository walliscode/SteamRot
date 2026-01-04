/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "TestFixture.h"
#include "TitleScene.h"
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

TEST_CASE("SceneFactory::CreateSceneFromData handles empty SceneData",
          "[SceneFactory]") {
  // Arrange
  steamrot::tests::TestFixture test_fixture;
  steamrot::SceneFactory scene_factory(test_fixture.GetGameContext());
  // Act
  auto result = scene_factory.CreateSceneFromData(nullptr);
  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message ==
          "SceneData pointer is null in SceneFactory::CreateSceneFromData");
}

TEST_CASE("SceneFactory::CreateSceneFromData creates Scene with valid "
          "FbsSceneData",
          "[SceneFactory]") {
  // Load test data from JSON (compiled to binary)
  auto [data_buffer, scene_data_fbs] = LoadSceneTestData();
  REQUIRE(scene_data_fbs != nullptr);

  // Verify the loaded data structure
  REQUIRE(scene_data_fbs->scene_info() != nullptr);
  REQUIRE(scene_data_fbs->scene_info()->scene_type() ==
          steamrot::SceneType::SceneType_TITLE);
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

  // Create FbsSceneData wrapper
  auto fbs_scene_data = std::make_unique<steamrot::FbsSceneData>();
  fbs_scene_data->scene_info.type = steamrot::SceneType::SceneType_TITLE;
  fbs_scene_data->entity_collection = scene_data_fbs->entity_collection();

  // Check scene state before CreateSceneFromData
  steamrot::tests::TestFixture test_fixture;
  steamrot::SceneFactory scene_factory(test_fixture.GetGameContext());

  // Verify scene_data before passing to factory
  REQUIRE(fbs_scene_data->scene_info.type ==
          steamrot::SceneType::SceneType_TITLE);
  REQUIRE(fbs_scene_data->entity_collection != nullptr);
  REQUIRE(fbs_scene_data->entity_collection->entity_memory_pool_size() == 50);
  REQUIRE(fbs_scene_data->entity_collection->entities()->size() == 2);

  // Act - Create scene from data
  auto result = scene_factory.CreateSceneFromData(std::move(fbs_scene_data));

  // Assert - Check scene was created successfully
  REQUIRE(result.has_value());
  auto &scene = result.value();
  REQUIRE(scene != nullptr);
  REQUIRE(dynamic_cast<steamrot::TitleScene *>(scene.get()) != nullptr);

  // Verify scene type matches
  REQUIRE(scene->GetSceneInfo().type == steamrot::SceneType::SceneType_TITLE);
}

TEST_CASE("SceneFactory::CreateSceneFromDefault creates TestScene with valid "
          "FbsSceneData",
          "[SceneFactory]") {
  // Arrange
  steamrot::tests::TestFixture test_fixture;
  steamrot::SceneFactory scene_factory(test_fixture.GetGameContext());
  // Act
  auto result = scene_factory.CreateSceneFromDefault(
      steamrot::SceneType::SceneType_TITLE);
  // Assert
  REQUIRE(result.has_value());
  REQUIRE(dynamic_cast<steamrot::TitleScene *>(result.value().get()) !=
          nullptr);
}
