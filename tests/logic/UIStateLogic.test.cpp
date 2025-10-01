/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIStateLogic class
/////////////////////////////////////////////////

#include "UIStateLogic.h"
#include "TestContext.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIStateLogic::UIStateLogic Constructor", "[UIStateLogic]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;

  steamrot::UIStateLogic ui_state_logic(
      test_context.GetLogicContextForTestScene());
  SUCCEED("UIStateLogic instantiated successfully");
}

TEST_CASE("UIStateLogic::ProcessLogic runs without errors on test scene",
          "[UIStateLogic]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;

  steamrot::UIStateLogic ui_state_logic(
      test_context.GetLogicContextForTestScene());

  // Should not crash even if there are no CUIState components
  REQUIRE_NOTHROW(ui_state_logic.RunLogic());
}
