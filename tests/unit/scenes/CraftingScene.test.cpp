/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CraftingScene
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "SceneFactory.h"
#include "TestFixture.h"
#include "draw_ui_elements_test_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CraftingScene's call to sRender is correct",
          "[unit][CraftingScene][.visual]") {
  // arrange
  steamrot::TestPaths test_paths;
  steamrot::tests::TestFixture test_context;
  steamrot::SceneFactory scene_factory;
  // create a CraftingScene
  auto scene_creation_result = scene_factory.CreateDefaultScene(
      steamrot::SceneType::SceneType_CRAFTING, test_context.GetGameContext());
  ;
  if (!scene_creation_result.has_value()) {
    FAIL("Scene creation failed: " + scene_creation_result.error().message);
  }
  // configure the scene from default
  auto crafting_scene = std::move(scene_creation_result.value());
  auto configure_result = crafting_scene->ConfigureFromDefault();
  if (!configure_result.has_value()) {
    FAIL("Scene configuration failed: " + configure_result.error().message);
  }
  // add the LogicMap
  steamrot::FlatbuffersDataLoader data_loader;
  auto logic_collection_data_result = data_loader.ProvideLogicCollectionData(
      steamrot::SceneType::SceneType_CRAFTING);
  if (!logic_collection_data_result.has_value()) {
    FAIL("Failed to load LogicCollectionData: " +
         logic_collection_data_result.error().message);
  }

  steamrot::LogicFactory logic_factory(steamrot::SceneType::SceneType_CRAFTING,
                                       crafting_scene->GetSceneContext());
  auto create_map_result =
      logic_factory.CreateLogicMap(logic_collection_data_result.value());
  if (!create_map_result) {
    FAIL("Logic map creation failed: " + create_map_result.error().message);
  }
  // pass the created logic map to the scene
  crafting_scene->SetLogicMap(std::move(create_map_result.value()));
  // call sRender
  REQUIRE_NOTHROW(crafting_scene->sRender());
  // evaluate render texture visually
  steamrot::tests::DisplayRenderTexture(crafting_scene->GetRenderTexture());
}
