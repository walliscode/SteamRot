/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CUserInterface class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CUserInterface.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CUserInterface has correct default values",
          "[unit][Components][CUserInterface]") {
  steamrot::CUserInterface ui;

  // Test pre configuration state
  REQUIRE(ui.m_active == false);
  REQUIRE(ui.m_name == "Default UI");
  REQUIRE(ui.m_root_element == nullptr);
  REQUIRE(ui.m_visible == false);
  REQUIRE(ui.m_priority_tier == steamrot::UIPriorityTier::Normal);
}
