/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIRenderLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "TestFixture.h"
#include "draw_ui_elements_test_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIRenderLogic::UIRenderLogic Constructor", "[unit][UIRenderLogic]") {
  // Create a dummy SceneContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context;
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetSceneContext());

  SUCCEED("UIRenderLogic instantiated successfully");
}

TEST_CASE("UIRenderLogic draws default for test scene", "[unit][UIRenderLogic][visual]") {
  // Create a dummy SceneContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context;

  // size the render texture and clear it
  bool resize_result =
      test_context.GetSceneContext().scene_texture.resize(
          {900, 600});
  REQUIRE(resize_result == true);

  test_context.GetSceneContext().scene_texture.clear(
      sf::Color::Black);

  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetSceneContext());
  // Call the draw method
  ui_render_logic.RunLogic();

  // view the render texture
  steamrot::tests::DisplayRenderTexture(
      test_context.GetSceneContext().scene_texture);
}

TEST_CASE("UIRenderLogic draws title scene", "[unit][UIRenderLogic][visual]") {
  // Create a dummy SceneContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context{
      steamrot::SceneType::SceneType_TITLE};
  // size the render texture and clear it
  bool resize_result =
      test_context.GetSceneContext().scene_texture.resize(
          {900, 600});
  REQUIRE(resize_result == true);
  test_context.GetSceneContext().scene_texture.clear(
      sf::Color::Black);
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetSceneContext());
  // Call the draw method
  ui_render_logic.RunLogic();
  // view the render texture
  steamrot::tests::DisplayRenderTexture(
      test_context.GetSceneContext().scene_texture);
}

TEST_CASE("UIRenderLogic draws crafting scene", "[unit][UIRenderLogic][visual]") {
  // Create a dummy SceneContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context{
      steamrot::SceneType::SceneType_CRAFTING};
  // size the render texture and clear it
  bool resize_result =
      test_context.GetSceneContext().scene_texture.resize(
          {900, 600});
  REQUIRE(resize_result == true);
  test_context.GetSceneContext().scene_texture.clear(
      sf::Color::Black);
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetSceneContext());
  // Call the draw method
  ui_render_logic.RunLogic();
  // view the render texture
  steamrot::tests::DisplayRenderTexture(
      test_context.GetSceneContext().scene_texture);
}
