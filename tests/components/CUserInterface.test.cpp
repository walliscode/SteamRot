/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CUserInterface class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CUserInterface.h"
#include "component_test_mixin.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CUserInterface follows Component contract",
          "[unit][Components][CUserInterface]") {
  // CUserInterface has unique_ptr members, so skip copy tests
  steamrot::tests::ComponentTestMixin<steamrot::CUserInterface>::TestDefaultConstruction();
  steamrot::tests::ComponentTestMixin<steamrot::CUserInterface>::TestComponentRegisterIndex();
  steamrot::tests::ComponentTestMixin<steamrot::CUserInterface>::TestActivation();
}

TEST_CASE("CUserInterface has correct default values",
          "[unit][Components][CUserInterface]") {
  steamrot::CUserInterface ui;
  
  // Test pre configuration state
  REQUIRE(ui.m_active == false);
  REQUIRE(ui.m_name == "Default UI");
  REQUIRE(ui.m_root_element == nullptr);
  REQUIRE(ui.m_UI_visible == false);
}
