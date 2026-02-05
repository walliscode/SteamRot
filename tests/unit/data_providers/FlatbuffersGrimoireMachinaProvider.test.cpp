/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersGrimoireMachinaProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersGrimoireMachinaProvider.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("FlatbuffersGrimoireMachinProvider::ConfigureGrimoireMachina "
          "succesfully configures a GrimoireMachina",
          "[FlatbuffersGrimoireMachinProvider]") {
  steamrot::FlatbuffersGrimoireMachinaProvider provider;
  steamrot::GrimoireMachina grimoire_machina;

  auto result = provider.ConfigureGrimoireMachina(grimoire_machina);

  // check that the result is successful
  if (!result.has_value()) {
    FAIL("ConfigureGrimoireMachina failed with error: " +
         result.error().message);
  }

  // check for fragment names
  std::vector<std::string> expected_fragment_names = {"rectangle"};
  REQUIRE(grimoire_machina.m_all_fragments.size() ==
          expected_fragment_names.size());
  for (const auto &name : expected_fragment_names) {
    CAPTURE(name);
    REQUIRE(grimoire_machina.m_all_fragments.contains(name));
  }

  // check for joint names
  std::vector<std::string> expected_joint_names = {"square"};
  REQUIRE(grimoire_machina.m_all_joints.size() == expected_joint_names.size());
  for (const auto &name : expected_joint_names) {
    CAPTURE(name);
    REQUIRE(grimoire_machina.m_all_joints.contains(name));
  }
}
