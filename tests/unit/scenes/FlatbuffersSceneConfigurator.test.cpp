/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSceneConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneConfigurator.h"
#include "FailInfo.h"
#include "TestFixture.h"
#include "TestScene.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "scene_types_generated.h"
#include "uuid.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("FlatbuffersSceneConfigurator can be instantiated",
          "[FlatbuffersSceneConfigurator]") {
  steamrot::FlatbuffersSceneConfigurator configurator;
  REQUIRE(true); // If we reach this point, the instantiation was successful
}

TEST_CASE("FlatbuffersSceneConfigurator::ConfigureSceneInfo provides UUID if "
          "not present",
          "[FlatbuffersSceneConfigurator]") {
  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;

  steamrot::FlatbuffersSceneConfigurator configurator;

  auto scene = steamrot::tests::TestScene(fixture.GetGameContext());
  // create FbsSceneData with nil UUID
  steamrot::SceneData scene_data;
  scene_data.scene_info.type = steamrot::SceneType_TITLE;
  REQUIRE(scene.GetSceneInfo().id.is_nil());

  auto result = configurator.ConfigureSceneInfo(scene, scene_data);

  REQUIRE(result.has_value());
  std::cout << "Generated UUID: " << scene.GetSceneInfo().id << std::endl;
  REQUIRE(!scene.GetSceneInfo().id.is_nil());
  REQUIRE(scene.GetSceneInfo().type == steamrot::SceneType_TITLE);
}

// TEST_CASE("FlatbuffersSceneConfigurator::ConfigureSceneReources configures "
//           "the SceneResources struct",
//           "[FlatbuffersSceneConfigurator]") {
//   // set up fixtures and objects
//   steamrot::tests::TestFixture fixture;
//   steamrot::SceneFactory scene_factory{fixture.GetGameContext()};
//   steamrot::FlatbuffersSceneConfigurator configurator;
//   auto scene_creation_result =
//       scene_factory.CreateSceneFromDefault(steamrot::SceneType_TITLE);
//   if (!scene_creation_result.has_value()) {
//     FAIL(scene_creation_result.error().message);
//   }
//   auto &scene = scene_creation_result.value();
//
//   // create valid FbsSceneData
//   steamrot::FbsSceneData fbs_scene_data;
//   auto result = configurator.ConfigureSceneResources(*scene,
//   &fbs_scene_data); if (!result.has_value()) {
//     FAIL(result.error().message);
//   }
//
//   // REQUIRE(scene->GetSceneResources().scene_texture.getSize().x == 800);
//   // REQUIRE(scene->GetSceneResources().scene_texture.getSize().y == 600);
// }

TEST_CASE("FlatbuffersSceneConfigurator::ConfigureLogicMap configures the "
          "scenes logic map",
          "[FlatbuffersSceneConfigurator]") {

  // set up fixtures and objects
  steamrot::tests::TestFixture fixture;
  steamrot::tests::TestScene test_scene{fixture.GetGameContext()};
  steamrot::FlatbuffersSceneConfigurator configurator;

  // ensure logic map is empty
  REQUIRE(test_scene.GetSceneResources().logic_map.empty());

  // configure logic map
  auto result = configurator.ConfigureLogicMap(test_scene);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  // check that logic map is populated
  auto &logic_collection = test_scene.GetSceneResources().logic_map;
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
