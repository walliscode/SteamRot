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
