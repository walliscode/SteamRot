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
