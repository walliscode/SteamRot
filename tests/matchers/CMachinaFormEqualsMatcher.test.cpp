/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CMachinaFormEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CMachinaFormEqualsMatcher.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CMachinaFormEqualsMatcher works correctly",
          "[unit][Components][CMachinaForm][matcher]") {
  steamrot::CMachinaForm expected;
  expected.m_active = true;
  expected.m_fragments.push_back(steamrot::Fragment{});
  expected.m_joints.push_back(steamrot::Joint{});

  steamrot::CMachinaForm actual;

  SECTION("Matcher detects differences in m_active") {
    actual.m_active = false;
    actual.m_fragments.push_back(steamrot::Fragment{});
    actual.m_joints.push_back(steamrot::Joint{});
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCMachinaForm(expected));
  }

  SECTION("Matcher detects differences in m_fragments size") {
    actual.m_active = true;
    actual.m_joints.push_back(steamrot::Joint{});
    // actual has 0 fragments, expected has 1
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCMachinaForm(expected));
  }

  SECTION("Matcher detects differences in m_joints size") {
    actual.m_active = true;
    actual.m_fragments.push_back(steamrot::Fragment{});
    // actual has 0 joints, expected has 1
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCMachinaForm(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_active = true;
    actual.m_fragments.push_back(steamrot::Fragment{});
    actual.m_joints.push_back(steamrot::Joint{});
    REQUIRE_THAT(actual, steamrot::tests::EqualsCMachinaForm(expected));
  }
}

TEST_CASE("CMachinaFormEqualsMatcher describe is as expected on success",
          "[unit][Components][CMachinaForm][matcher]") {
  steamrot::CMachinaForm expected;
  expected.m_active = true;
  steamrot::CMachinaForm actual;
  actual.m_active = true;
  auto matcher = steamrot::tests::EqualsCMachinaForm(expected);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestPassed() + "CMachinaForm Match:", 3) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CMachinaFormEqualsMatcher describe is as expected on failure",
          "[unit][Components][CMachinaForm][matcher]") {
  steamrot::CMachinaForm expected;
  expected.m_active = true;
  expected.m_fragments.push_back(steamrot::Fragment{});
  
  steamrot::CMachinaForm actual;
  actual.m_active = false;

  auto matcher = steamrot::tests::EqualsCMachinaForm(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "CMachinaForm Match:", 3) << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed() << "m_active:"
      << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize(actual.m_active, conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize(expected.m_active, conmat::Color::Blue) << "\n";

  oss << conmat::Indent(1) << conmat::TestFailed()
      << "m_fragments size mismatch:" << "\n";
  oss << conmat::Indent(2) << "actual size: "
      << conmat::Colorize(actual.m_fragments.size(), conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected size: "
      << conmat::Colorize(expected.m_fragments.size(), conmat::Color::Blue)
      << "\n";

  REQUIRE(matcher.describe() == oss.str());
}
