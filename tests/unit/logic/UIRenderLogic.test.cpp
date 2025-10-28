/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIRenderLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "TestContext.h"
#include "draw_ui_elements_test_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIRenderLogic::UIRenderLogic Constructor", "[unit][UIRenderLogic]") {
  // Create a dummy SceneContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetSceneContextForTestScene());

  SUCCEED("UIRenderLogic instantiated successfully");
}

TEST_CASE("UIRenderLogic draws default for test scene", "[unit][UIRenderLogic]") {
  // Create a dummy SceneContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;

  // size the render texture and clear it
  bool resize_result =
      test_context.GetSceneContextForTestScene().scene_texture.resize(
          {900, 600});
  REQUIRE(resize_result == true);

  test_context.GetSceneContextForTestScene().scene_texture.clear(
      sf::Color::Black);

  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetSceneContextForTestScene());
  // Call the draw method
  ui_render_logic.RunLogic();

  // view the render texture
  steamrot::tests::DisplayRenderTexture(
      test_context.GetSceneContextForTestScene().scene_texture);
}

TEST_CASE("UIRenderLogic draws title scene", "[unit][UIRenderLogic]") {
  // Create a dummy SceneContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context{
      steamrot::SceneType::SceneType_TITLE};
  // size the render texture and clear it
  bool resize_result =
      test_context.GetSceneContextForTitleScene().scene_texture.resize(
          {900, 600});
  REQUIRE(resize_result == true);
  test_context.GetSceneContextForTitleScene().scene_texture.clear(
      sf::Color::Black);
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetSceneContextForTitleScene());
  // Call the draw method
  ui_render_logic.RunLogic();
  // view the render texture
  steamrot::tests::DisplayRenderTexture(
      test_context.GetSceneContextForTitleScene().scene_texture);
}

TEST_CASE("UIRenderLogic draws crafting scene", "[unit][UIRenderLogic]") {
  // Create a dummy SceneContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context{
      steamrot::SceneType::SceneType_CRAFTING};
  // size the render texture and clear it
  bool resize_result =
      test_context.GetSceneContextForCraftingScene().scene_texture.resize(
          {900, 600});
  REQUIRE(resize_result == true);
  test_context.GetSceneContextForCraftingScene().scene_texture.clear(
      sf::Color::Black);
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetSceneContextForCraftingScene());
  // Call the draw method
  ui_render_logic.RunLogic();
  // view the render texture
  steamrot::tests::DisplayRenderTexture(
      test_context.GetSceneContextForCraftingScene().scene_texture);
}
