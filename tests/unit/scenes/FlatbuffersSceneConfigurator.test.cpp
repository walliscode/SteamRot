/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSceneConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneConfigurator.h"
#include "FailInfo.h"
#include "FbsSceneData.h"
#include "FlatbuffersSceneDataProvider.h"
#include "SceneFactory.h"
#include "TestFixture.h"
#include "TestScene.h"
#include "entity_memory.h"
#include "scene_types_generated.h"
#include "uuid.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>

TEST_CASE("FlatbuffersSceneConfigurator can be instantiated",
          "[FlatbuffersSceneConfigurator]") {
  steamrot::FlatbuffersSceneConfigurator configurator;
  REQUIRE(true); // If we reach this point, the instantiation was successful
}
TEST_CASE(
    "FlatbuffersSceneConfigurator::ConfigureSceneInfo handles null SceneData",
    "[FlatbuffersSceneConfigurator]") {

  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::SceneFactory scene_factory{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;
  auto scene_creation_result =
      scene_factory.CreateSceneFromDefault(steamrot::SceneType_TITLE);
  if (!scene_creation_result.has_value()) {
    FAIL(scene_creation_result.error().message);
  }
  auto &scene = scene_creation_result.value();

  // pass null SceneData
  steamrot::SceneData *null_scene_data = nullptr;
  auto result = configurator.ConfigureSceneInfo(*scene, null_scene_data);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message == "SceneData pointer is null");
}

TEST_CASE("FlatbuffersSceneConfigurator::ConfigureSceneInfo handles wrong "
          "derived SceneData",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::SceneFactory scene_factory{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;
  auto scene_creation_result =
      scene_factory.CreateSceneFromDefault(steamrot::SceneType_TITLE);
  if (!scene_creation_result.has_value()) {
    FAIL(scene_creation_result.error().message);
  }
  auto &scene = scene_creation_result.value();

  // create a wrong derived SceneData type
  steamrot::SceneData wrong_scene_data; // Not of type FbsSceneData
  auto result = configurator.ConfigureSceneInfo(*scene, &wrong_scene_data);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidCast);
  REQUIRE(result.error().message == "SceneData is not FbsSceneData");
}

TEST_CASE("FlatbuffersSceneConfigurator::ConfigureSceneInfo provides UUID if "
          "not present",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;

  steamrot::FlatbuffersSceneConfigurator configurator;

  auto scene = steamrot::tests::TestScene(fixture.GetGameContext());
  // create FbsSceneData with nil UUID
  steamrot::FbsSceneData fbs_scene_data;
  fbs_scene_data.scene_info.type = steamrot::SceneType_TITLE;
  REQUIRE(scene.GetSceneInfo().id.is_nil());

  auto result = configurator.ConfigureSceneInfo(scene, &fbs_scene_data);

  REQUIRE(result.has_value());
  std::cout << "Generated UUID: " << scene.GetSceneInfo().id << std::endl;
  REQUIRE(!scene.GetSceneInfo().id.is_nil());
  REQUIRE(scene.GetSceneInfo().type == steamrot::SceneType_TITLE);
}

TEST_CASE("FlatbuffersSceneConfigurator::ConfigureSceneResources "
          "handles null "
          "SceneData",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::SceneFactory scene_factory{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;
  auto scene_creation_result =
      scene_factory.CreateSceneFromDefault(steamrot::SceneType_TITLE);
  if (!scene_creation_result.has_value()) {
    FAIL(scene_creation_result.error().message);
  }
  auto &scene = scene_creation_result.value();
  // pass null SceneData
  steamrot::SceneData *null_scene_data = nullptr;
  auto result = configurator.ConfigureSceneResources(*scene, null_scene_data);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message == "SceneData pointer is null");
}
TEST_CASE("FlatbuffersSceneConfigurator::ConfigureSceneResources handles "
          "wrong derived SceneData",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::SceneFactory scene_factory{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;
  auto scene_creation_result =
      scene_factory.CreateSceneFromDefault(steamrot::SceneType_TITLE);
  if (!scene_creation_result.has_value()) {
    FAIL(scene_creation_result.error().message);
  }
  auto &scene = scene_creation_result.value();
  // create a wrong derived SceneData type
  steamrot::SceneData wrong_scene_data; // Not of type FbsSceneData
  auto result = configurator.ConfigureSceneResources(*scene, &wrong_scene_data);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidCast);
  REQUIRE(result.error().message == "SceneData is not FbsSceneData");
}

TEST_CASE("FlatbuffersSceneConfigurator::ConfigureSceneConfig handles null "
          "SceneData",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::SceneFactory scene_factory{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;
  auto scene_creation_result =
      scene_factory.CreateSceneFromDefault(steamrot::SceneType_TITLE);
  if (!scene_creation_result.has_value()) {
    FAIL(scene_creation_result.error().message);
  }
  auto &scene = scene_creation_result.value();
  // pass null SceneData
  steamrot::SceneData *null_scene_data = nullptr;
  auto result = configurator.ConfigureSceneConfig(*scene, null_scene_data);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message == "SceneData pointer is null");
}

TEST_CASE("FlatbuffersSceneConfigurator::ConfigureSceneConfig "
          "handles wrong "
          "derived SceneData",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::SceneFactory scene_factory{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;
  auto scene_creation_result =
      scene_factory.CreateSceneFromDefault(steamrot::SceneType_TITLE);
  if (!scene_creation_result.has_value()) {
    FAIL(scene_creation_result.error().message);
  }
  auto &scene = scene_creation_result.value();
  // create a wrong derived SceneData type
  steamrot::SceneData wrong_scene_data; // Not of type FbsSceneData

  auto result = configurator.ConfigureSceneConfig(*scene, &wrong_scene_data);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidCast);
  REQUIRE(result.error().message == "SceneData is not FbsSceneData");
}

TEST_CASE("FlatbuffersSceneConfigurator::ConfigureEntities handles null "
          "SceneData",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::tests::TestScene test_scene{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;

  // pass null SceneData
  steamrot::SceneData *null_scene_data = nullptr;
  auto result = configurator.ConfigureEntities(test_scene, null_scene_data);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message == "SceneData pointer is null");
}

TEST_CASE("FlatbuffersSceneConfigurator::ConfigureEntities handles wrong "
          "derived SceneData",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::tests::TestScene test_scene{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;
  // create a wrong derived SceneData type
  steamrot::SceneData wrong_scene_data; // Not of type FbsSceneData
  auto result = configurator.ConfigureEntities(test_scene, &wrong_scene_data);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidCast);
  REQUIRE(result.error().message == "SceneData is not FbsSceneData");
}

TEST_CASE(
    "FlatbuffersSceneConfigurator::ConfigureEntities hanldes empty entity data",
    "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::tests::TestScene test_scene{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;
  steamrot::FbsSceneData fbs_scene_data; // empty entity data
  auto result = configurator.ConfigureEntities(test_scene, &fbs_scene_data);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "SceneDataFbs not found");
}

TEST_CASE("FlatbuffersSceneConfigurator::ConfiguresEntities modifies the "
          "EntityMemoryPool",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::tests::TestScene test_scene{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneDataProvider data_provider;
  steamrot::FlatbuffersSceneConfigurator configurator;

  // get initial entity count
  size_t initial_entity_count = steamrot::entity::memory::GetMemoryPoolSize(
      test_scene.GetSceneContext().scene_entities);
  REQUIRE(initial_entity_count == 0);
  // load scene data
  auto scene_data_result =
      data_provider.ProvideDefaultSceneData(steamrot::SceneType_TEST);
  if (!scene_data_result.has_value()) {
    FAIL(scene_data_result.error().message);
  }
  // configure entities via configurator
  auto configure_result = configurator.ConfigureEntities(
      test_scene, scene_data_result.value().get());

  if (!configure_result.has_value()) {
    FAIL(configure_result.error().message);
  }

  // get final entity count
  size_t final_entity_count = steamrot::entity::memory::GetMemoryPoolSize(
      test_scene.GetSceneContext().scene_entities);
  REQUIRE(final_entity_count == 146);
}
