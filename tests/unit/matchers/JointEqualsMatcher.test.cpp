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
  expected.name = "test_joint";

  steamrot::Joint actual;

  SECTION("Matcher detects differences in name") {
    actual.name = "different_joint";
    REQUIRE_THAT(actual, !steamrot::tests::EqualsJoint(expected));
  }

  SECTION("Matcher detects equality") {
    actual.name = "test_joint";
    REQUIRE_THAT(actual, steamrot::tests::EqualsJoint(expected));
  }
}

TEST_CASE("JointEqualsMatcher describe is as expected on success",
          "[unit][Joint][matcher]") {
  steamrot::Joint expected;
  expected.name = "test_joint";
  steamrot::Joint actual;
  actual.name = "test_joint";
  auto matcher = steamrot::tests::EqualsJoint(expected);

  REQUIRE(matcher.describe() == "equals Joint(name='test_joint')");
}

TEST_CASE("JointEqualsMatcher describe is as expected on failure",
          "[unit][Joint][matcher]") {
  steamrot::Joint expected;
  expected.name = "expected_joint";

  steamrot::Joint actual;
  actual.name = "actual_joint";

  auto matcher = steamrot::tests::EqualsJoint(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("Joint mismatch:") != std::string::npos);
  REQUIRE(description.find("name:") != std::string::npos);
}
