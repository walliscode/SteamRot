/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FragmentEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FragmentEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FragmentEqualsMatcher works correctly",
          "[unit][Fragment][matcher]") {
  steamrot::Fragment expected;
  expected.m_name = "test_fragment";

  steamrot::Fragment actual;

  SECTION("Matcher detects differences in m_name") {
    actual.m_name = "different_name";
    REQUIRE_THAT(actual, !steamrot::tests::EqualsFragment(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_name = "test_fragment";
    REQUIRE_THAT(actual, steamrot::tests::EqualsFragment(expected));
  }
}

TEST_CASE("FragmentEqualsMatcher describe is as expected on success",
          "[unit][Fragment][matcher]") {
  steamrot::Fragment expected;
  expected.m_name = "test_fragment";
  steamrot::Fragment actual;
  actual.m_name = "test_fragment";
  auto matcher = steamrot::tests::EqualsFragment(expected);

  REQUIRE(matcher.describe() == "equals Fragment(m_name='test_fragment')");
}

TEST_CASE("FragmentEqualsMatcher describe is as expected on failure",
          "[unit][Fragment][matcher]") {
  steamrot::Fragment expected;
  expected.m_name = "expected_name";
  steamrot::Fragment actual;
  actual.m_name = "actual_name";

  auto matcher = steamrot::tests::EqualsFragment(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("Fragment mismatch:") != std::string::npos);
  REQUIRE(description.find("m_name:") != std::string::npos);
}
