/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CGrimoireMachina class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachina.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CGrimoireMachina has correct default values",
          "[unit][Components][CGrimoireMachina]") {
  steamrot::CGrimoireMachina grimoire;

  // Test default state
  REQUIRE(grimoire.m_all_fragments.empty());
  REQUIRE(grimoire.m_all_joints.empty());
  REQUIRE(grimoire.m_machina_forms.empty());
  REQUIRE(grimoire.m_holding_form == nullptr);
}
