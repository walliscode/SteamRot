/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIStateLogic class
/////////////////////////////////////////////////

#include "UIStateLogic.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIStateLogic::UIStateLogic Constructor", "[unit][UIStateLogic]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context;

  steamrot::UIStateLogic ui_state_logic(
      test_context.GetSceneContext());
  SUCCEED("UIStateLogic instantiated successfully");
}

TEST_CASE("UIStateLogic::ProcessLogic runs without errors on test scene",
          "[unit][UIStateLogic]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context;

  steamrot::UIStateLogic ui_state_logic(
      test_context.GetSceneContext());

  // Should not crash even if there are no CUIState components
  REQUIRE_NOTHROW(ui_state_logic.RunLogic());
}
