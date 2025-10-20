/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CGrimoireMachina class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachina.h"
#include "component_test_mixin.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CGrimoireMachina follows Component contract",
          "[unit][Components][CGrimoireMachina]") {
  // CGrimoireMachina has unique_ptr members, so skip copy tests
  steamrot::tests::ComponentTestMixin<steamrot::CGrimoireMachina>::TestDefaultConstruction();
  steamrot::tests::ComponentTestMixin<steamrot::CGrimoireMachina>::TestComponentRegisterIndex();
  steamrot::tests::ComponentTestMixin<steamrot::CGrimoireMachina>::TestActivation();
}

TEST_CASE("CGrimoireMachina has correct default values",
          "[unit][Components][CGrimoireMachina]") {
  steamrot::CGrimoireMachina grimoire;
  
  // Test pre configuration state
  REQUIRE(grimoire.m_active == false);
  REQUIRE(grimoire.m_all_fragments.empty());
  REQUIRE(grimoire.m_all_joints.empty());
  REQUIRE(grimoire.m_machina_forms.empty());
  REQUIRE(grimoire.m_holding_form == nullptr);
}
