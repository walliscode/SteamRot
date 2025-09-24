/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIRenderLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "TestContext.h"
#include "draw_ui_elements_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIRenderLogic::UIRenderLogic Constructor", "[UIRenderLogic]") {
  // Create a dummy LogicContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetLogicContextForTestScene());

  SUCCEED("UIRenderLogic instantiated successfully");
}

TEST_CASE("UIRenderLogic draws default for test scene", "[UIRenderLogic]") {
  // Create a dummy LogicContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;

  // size the render texture and clear it
  bool resize_result =
      test_context.GetLogicContextForTestScene().scene_texture.resize(
          {900, 600});
  REQUIRE(resize_result == true);

  test_context.GetLogicContextForTestScene().scene_texture.clear(
      sf::Color::Black);

  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetLogicContextForTestScene());
  // Call the draw method
  ui_render_logic.RunLogic();

  // view the render texture
  steamrot::tests::DisplayRenderTexture(
      test_context.GetLogicContextForTestScene().scene_texture);
}

TEST_CASE("UIRenderLogic draws title scene", "[UIRenderLogic]") {
  // Create a dummy LogicContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context{
      steamrot::SceneType::SceneType_TITLE};
  // size the render texture and clear it
  bool resize_result =
      test_context.GetLogicContextForTitleScene().scene_texture.resize(
          {900, 600});
  REQUIRE(resize_result == true);
  test_context.GetLogicContextForTitleScene().scene_texture.clear(
      sf::Color::Black);
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetLogicContextForTitleScene());
  // Call the draw method
  ui_render_logic.RunLogic();
  // view the render texture
  steamrot::tests::DisplayRenderTexture(
      test_context.GetLogicContextForTitleScene().scene_texture);
}

TEST_CASE("UIRenderLogic draws crafting scene", "[UIRenderLogic]") {
  // Create a dummy LogicContext
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context{
      steamrot::SceneType::SceneType_CRAFTING};
  // size the render texture and clear it
  bool resize_result =
      test_context.GetLogicContextForCraftingScene().scene_texture.resize(
          {900, 600});
  REQUIRE(resize_result == true);
  test_context.GetLogicContextForCraftingScene().scene_texture.clear(
      sf::Color::Black);
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(
      test_context.GetLogicContextForCraftingScene());
  // Call the draw method
  ui_render_logic.RunLogic();
  // view the render texture
  steamrot::tests::DisplayRenderTexture(
      test_context.GetLogicContextForCraftingScene().scene_texture);
}
