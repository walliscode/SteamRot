/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIRenderLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "TestContext.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIRenderLogic::UIRenderLogic Constructor", "[UIRenderLogic]") {
  // Create a dummy LogicContext
  steamrot::tests::TestContext test_context;
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(test_context.GetLogicContext());

  SUCCEED("UIRenderLogic instantiated successfully");
}

TEST_CASE("UIRenderLogic::PopulateUIStyles", "[UIRenderLogic]") {
  // Create a dummy LogicContext
  steamrot::tests::TestContext test_context;
  // Instantiate UIRenderLogic
  steamrot::UIRenderLogic ui_render_logic(test_context.GetLogicContext());

  REQUIRE(!ui_render_logic.GetUIStyles().empty());

  std::vector<std::string> expected_style_names = {"default"};
  for (const auto &style_name : expected_style_names) {
    REQUIRE(ui_render_logic.GetUIStyles().find(style_name) !=
            ui_render_logic.GetUIStyles().end());
  }
}
