/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for TitleScene
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "SceneFactory.h"
#include "TestEngine.h"
#include "draw_ui_elements_test_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TitleScene's call of sRender is correct",
          "[unit][TitleScene][.visual]") {

  // arrange
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestEngine test_engine(nullptr);
  auto init_result = test_engine.Initialize();
  REQUIRE(init_result.has_value());
  steamrot::SceneFactory scene_factory;

  // create a TitleScene
  auto scene_creation_result = scene_factory.CreateDefaultScene(
      steamrot::SceneType::SceneType_TITLE, test_engine.GetGameContext());
  ;

  if (!scene_creation_result.has_value()) {
    FAIL("Scene creation failed: " + scene_creation_result.error().message);
  }

  // configure the scene from default
  auto title_scene = std::move(scene_creation_result.value());
  auto configure_result = title_scene->ConfigureFromDefault();
  if (!configure_result.has_value()) {
    FAIL("Scene configuration failed: " + configure_result.error().message);
  }
  // add the LogicMap
  steamrot::FlatbuffersDataLoader data_loader;
  auto logic_collection_data_result = data_loader.ProvideLogicCollectionData(
      steamrot::SceneType::SceneType_TITLE);
  if (!logic_collection_data_result.has_value()) {
    FAIL("Failed to load LogicCollectionData: " +
         logic_collection_data_result.error().message);
  }

  steamrot::LogicFactory logic_factory(steamrot::SceneType::SceneType_TITLE,
                                       title_scene->GetSceneContext());

  auto create_map_result =
      logic_factory.CreateLogicMap(logic_collection_data_result.value());
  if (!create_map_result) {
    FAIL("Logic map creation failed: " + create_map_result.error().message);
  }
  // pass the created logic map to the scene
  title_scene->SetLogicMap(std::move(create_map_result.value()));
  // call sRender

  REQUIRE_NOTHROW(title_scene->sRender());

  // evaluate render texture visually
  steamrot::tests::DisplayRenderTexture(title_scene->GetRenderTexture());
}
