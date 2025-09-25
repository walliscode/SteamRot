/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for TitleScene
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "TestContext.h"
#include "draw_ui_elements_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TitleScene's call of sRender is correct", "[TitleScene]") {

  // arrange
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  steamrot::SceneFactory scene_factory;

  // create a TitleScene
  auto scene_creation_result = scene_factory.CreateDefaultScene(
      steamrot::SceneType::SceneType_TITLE, test_context.GetGameContext());
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
  steamrot::LogicFactory logic_factory(steamrot::SceneType::SceneType_TITLE,
                                       title_scene->GetLogicContext());

  auto create_map_result = logic_factory.CreateLogicMap();
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
