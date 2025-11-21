/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CGrimoireMachinaEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachinaEqualsMatcher.h"
#include "Fragment.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CGrimoireMachinaEqualsMatcher works correctly",
          "[unit][Components][CGrimoireMachina][matcher]") {
  steamrot::CGrimoireMachina expected;
  expected.m_active = true;

  steamrot::CGrimoireMachina actual;

  SECTION("Matcher detects differences in m_active") {
    actual.m_active = false;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCGrimoireMachina(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_active = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCGrimoireMachina(expected));
  }
}

TEST_CASE("CGrimoireMachinaEqualsMatcher describe is as expected on success",
          "[unit][Components][CGrimoireMachina][matcher]") {
  steamrot::CGrimoireMachina expected;
  expected.m_active = true;
  steamrot::CGrimoireMachina actual;
  actual.m_active = true;
  auto matcher = steamrot::tests::EqualsCGrimoireMachina(expected);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestPassed() + "CGrimoireMachina Match:", 3)
      << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CGrimoireMachinaEqualsMatcher describe is as expected on failure",
          "[unit][Components][CGrimoireMachina][matcher]") {
  steamrot::CGrimoireMachina expected;
  expected.m_active = true;
  steamrot::CGrimoireMachina actual;
  actual.m_active = false;

  auto matcher = steamrot::tests::EqualsCGrimoireMachina(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "CGrimoireMachina Match:", 3)
      << "\n";
  oss << "\t" << conmat::TestFailed() << "m_active:"
      << "\n";
  oss << "\t\t"
      << "actual = " << actual.m_active << "\n";
  oss << "\t\t"
      << "expected = " << expected.m_active << "\n";
  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE(
    "CGrimoireMachinaEqualsMatcher detects fragment size differences",
    "[unit][Components][CGrimoireMachina][matcher]") {
  steamrot::CGrimoireMachina expected;
  expected.m_active = true;
  expected.m_all_fragments["frag1"] = steamrot::Fragment{};
  expected.m_all_fragments["frag2"] = steamrot::Fragment{};
  expected.m_all_fragments["frag3"] = steamrot::Fragment{};
  expected.m_all_fragments["frag4"] = steamrot::Fragment{};
  expected.m_all_fragments["frag5"] = steamrot::Fragment{};

  steamrot::CGrimoireMachina actual;
  actual.m_active = true;
  actual.m_all_fragments["frag1"] = steamrot::Fragment{};
  actual.m_all_fragments["frag2"] = steamrot::Fragment{};
  actual.m_all_fragments["frag3"] = steamrot::Fragment{};

  REQUIRE_THAT(actual, !steamrot::tests::EqualsCGrimoireMachina(expected));
}

TEST_CASE("CGrimoireMachinaEqualsMatcher detects joint size differences",
          "[unit][Components][CGrimoireMachina][matcher]") {
  steamrot::CGrimoireMachina expected;
  expected.m_active = true;
  expected.m_all_joints["joint1"] = steamrot::Joint{};
  expected.m_all_joints["joint2"] = steamrot::Joint{};

  steamrot::CGrimoireMachina actual;
  actual.m_active = true;
  actual.m_all_joints["joint1"] = steamrot::Joint{};
  actual.m_all_joints["joint2"] = steamrot::Joint{};
  actual.m_all_joints["joint3"] = steamrot::Joint{};
  actual.m_all_joints["joint4"] = steamrot::Joint{};

  REQUIRE_THAT(actual, !steamrot::tests::EqualsCGrimoireMachina(expected));
}
