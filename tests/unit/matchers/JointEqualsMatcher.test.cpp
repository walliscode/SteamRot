/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for JointEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("JointEqualsMatcher works correctly", "[unit][Joint][matcher]") {
  steamrot::Joint expected;
  expected.m_joint_name = "test_joint";
  expected.m_number_of_connections = 2;

  steamrot::Joint actual;

  SECTION("Matcher detects differences in m_joint_name") {
    actual.m_joint_name = "different_joint";
    actual.m_number_of_connections = 2;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsJoint(expected));
  }

  SECTION("Matcher detects differences in m_number_of_connections") {
    actual.m_joint_name = "test_joint";
    actual.m_number_of_connections = 3;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsJoint(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_joint_name = "test_joint";
    actual.m_number_of_connections = 2;
    REQUIRE_THAT(actual, steamrot::tests::EqualsJoint(expected));
  }
}

TEST_CASE("JointEqualsMatcher describe is as expected on success",
          "[unit][Joint][matcher]") {
  steamrot::Joint expected;
  expected.m_joint_name = "test_joint";
  steamrot::Joint actual;
  actual.m_joint_name = "test_joint";
  auto matcher = steamrot::tests::EqualsJoint(expected);

  REQUIRE(matcher.describe() == "equals Joint(m_joint_name='test_joint')");
}

TEST_CASE("JointEqualsMatcher describe is as expected on failure",
          "[unit][Joint][matcher]") {
  steamrot::Joint expected;
  expected.m_joint_name = "expected_joint";
  expected.m_number_of_connections = 2;
  
  steamrot::Joint actual;
  actual.m_joint_name = "actual_joint";
  actual.m_number_of_connections = 3;

  auto matcher = steamrot::tests::EqualsJoint(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("Joint mismatch:") != std::string::npos);
  REQUIRE(description.find("m_joint_name:") != std::string::npos);
}
