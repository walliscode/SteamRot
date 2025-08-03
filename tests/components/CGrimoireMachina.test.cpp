/////////////////////////////////////////////////
/// @file
/// @brief Units tests for CGrimoireMachina class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachina.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Configuring a CGrimoireMachina turns it active",
          "[Components][CGrimoireMachina]") {

  steamrot::CGrimoireMachina grimoire;
  // Test pre configuration state
  REQUIRE(grimoire.m_active == false);
  REQUIRE(grimoire.m_all_fragments.empty());
  REQUIRE(grimoire.m_all_joints.empty());
  REQUIRE(grimoire.m_machina_forms.empty());
  REQUIRE(grimoire.m_holding_form == nullptr);
  REQUIRE(grimoire.GetComponentRegisterIndex() == 3);
}
