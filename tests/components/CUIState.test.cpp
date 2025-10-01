/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CUIState class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CUIState.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CUIState is default constructible and has correct properties",
          "[Components][CUIState]") {

  steamrot::CUIState ui_state;
  // Test pre configuration state
  REQUIRE(ui_state.m_active == false);
  REQUIRE(ui_state.m_state_to_ui_visibility.empty());
  REQUIRE(ui_state.m_state_values.empty());
  REQUIRE(ui_state.m_state_subscribers.empty());
  REQUIRE(ui_state.GetComponentRegisterIndex() == 4);
}
