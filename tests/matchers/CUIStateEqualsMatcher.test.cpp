/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CUIStateEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CUIStateEqualsMatcher.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CUIStateEqualsMatcher works correctly",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;

  steamrot::CUIState actual;

  SECTION("Matcher detects differences in m_active") {
    actual.m_active = false;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_active = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
  }
}

TEST_CASE("CUIStateEqualsMatcher describe is as expected on success",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  steamrot::CUIState actual;
  actual.m_active = true;
  auto matcher = steamrot::tests::EqualsCUIState(expected);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestPassed() + "CUIState Match:", 3) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CUIStateEqualsMatcher describe is as expected on failure",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  steamrot::CUIState actual;
  actual.m_active = false;

  auto matcher = steamrot::tests::EqualsCUIState(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "CUIState Match:", 3) << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed() << "m_active:"
      << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize(actual.m_active, conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize(expected.m_active, conmat::Color::Blue) << "\n";
  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CUIStateEqualsMatcher detects state_to_ui_visibility size "
          "differences",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  expected.m_state_to_ui_visibility["state2"] = steamrot::UIVisibilityState{};

  steamrot::CUIState actual;
  actual.m_active = true;
  actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};

  REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
}

TEST_CASE("CUIStateEqualsMatcher detects state_subscribers size differences",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_subscribers["key1"] = {};
  expected.m_state_subscribers["key2"] = {};
  expected.m_state_subscribers["key3"] = {};

  steamrot::CUIState actual;
  actual.m_active = true;
  actual.m_state_subscribers["key1"] = {};

  REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
}
