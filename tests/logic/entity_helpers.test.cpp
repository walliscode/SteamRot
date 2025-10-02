/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for entity_helpers namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entity_helpers.h"
#include "CGrimoireMachina.h"
#include "Fragment.h"
#include "Joint.h"
#include "PathProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetAllFragmentNames returns empty vector for empty fragments map",
          "[entity_helpers]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::CGrimoireMachina grimoire;

  std::vector<std::string> fragment_names =
      steamrot::entity_helpers::GetAllFragmentNames(grimoire);

  REQUIRE(fragment_names.empty());
}

TEST_CASE("GetAllFragmentNames returns all fragment names",
          "[entity_helpers]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::CGrimoireMachina grimoire;

  // Add some fragments
  steamrot::Fragment fragment1;
  fragment1.m_name = "fragment_a";
  grimoire.m_all_fragments["fragment_a"] = fragment1;

  steamrot::Fragment fragment2;
  fragment2.m_name = "fragment_b";
  grimoire.m_all_fragments["fragment_b"] = fragment2;

  steamrot::Fragment fragment3;
  fragment3.m_name = "fragment_c";
  grimoire.m_all_fragments["fragment_c"] = fragment3;

  std::vector<std::string> fragment_names =
      steamrot::entity_helpers::GetAllFragmentNames(grimoire);

  REQUIRE(fragment_names.size() == 3);
  REQUIRE(std::find(fragment_names.begin(), fragment_names.end(),
                    "fragment_a") != fragment_names.end());
  REQUIRE(std::find(fragment_names.begin(), fragment_names.end(),
                    "fragment_b") != fragment_names.end());
  REQUIRE(std::find(fragment_names.begin(), fragment_names.end(),
                    "fragment_c") != fragment_names.end());
}

TEST_CASE("GetAllJointNames returns empty vector for empty joints map",
          "[entity_helpers]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::CGrimoireMachina grimoire;

  std::vector<std::string> joint_names =
      steamrot::entity_helpers::GetAllJointNames(grimoire);

  REQUIRE(joint_names.empty());
}

TEST_CASE("GetAllJointNames returns all joint names", "[entity_helpers]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::CGrimoireMachina grimoire;

  // Add some joints
  steamrot::Joint joint1;
  joint1.m_joint_name = "joint_a";
  grimoire.m_all_joints["joint_a"] = joint1;

  steamrot::Joint joint2;
  joint2.m_joint_name = "joint_b";
  grimoire.m_all_joints["joint_b"] = joint2;

  steamrot::Joint joint3;
  joint3.m_joint_name = "joint_c";
  grimoire.m_all_joints["joint_c"] = joint3;

  std::vector<std::string> joint_names =
      steamrot::entity_helpers::GetAllJointNames(grimoire);

  REQUIRE(joint_names.size() == 3);
  REQUIRE(std::find(joint_names.begin(), joint_names.end(), "joint_a") !=
          joint_names.end());
  REQUIRE(std::find(joint_names.begin(), joint_names.end(), "joint_b") !=
          joint_names.end());
  REQUIRE(std::find(joint_names.begin(), joint_names.end(), "joint_c") !=
          joint_names.end());
}
