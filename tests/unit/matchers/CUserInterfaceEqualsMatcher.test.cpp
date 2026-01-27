/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CUserInterfaceEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CUserInterfaceEqualsMatcher.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CUserInterfaceEqualsMatcher works correctly",
          "[unit][Components][CUserInterface][matcher]") {
  steamrot::CUserInterface expected;
  expected.m_active = true;
  expected.m_name = "test_ui";

  steamrot::CUserInterface actual;

  SECTION("Matcher detects differences in m_active") {
    actual.m_active = false;
    actual.m_name = "test_ui";
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUserInterface(expected));
  }

  SECTION("Matcher detects differences in m_name") {
    actual.m_active = true;
    actual.m_name = "other_ui";
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUserInterface(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_active = true;
    actual.m_name = "test_ui";
    REQUIRE_THAT(actual, steamrot::tests::EqualsCUserInterface(expected));
  }
}

TEST_CASE("CUserInterfaceEqualsMatcher describe is as expected on success",
          "[unit][Components][CUserInterface][matcher]") {
  steamrot::CUserInterface expected;
  expected.m_active = true;
  expected.m_name = "test_ui";
  steamrot::CUserInterface actual;
  actual.m_active = true;
  actual.m_name = "test_ui";
  auto matcher = steamrot::tests::EqualsCUserInterface(expected);

  std::ostringstream oss;
  oss << conmat::Header(
             conmat::TestPassed() + " Entity [?] " +
                 std::format("CUserInterface ({}) Match:", expected.m_name),
             3)
      << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CUserInterfaceEqualsMatcher describe is as expected on failure",
          "[unit][Components][CUserInterface][matcher]") {
  steamrot::CUserInterface expected;
  expected.m_active = true;
  expected.m_name = "test_ui";
  steamrot::CUserInterface actual;
  actual.m_active = false;
  actual.m_name = "other_ui";

  auto matcher = steamrot::tests::EqualsCUserInterface(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(
             conmat::TestFailed() + " Entity [?] " +
                 std::format("CUserInterface ({}) Match:", expected.m_name),
             3)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed() << "m_active:"
      << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize(actual.m_active, conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize(expected.m_active, conmat::Color::Blue) << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed() << "m_name:"
      << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize(actual.m_name, conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2)
      << "expected: " << conmat::Colorize(expected.m_name, conmat::Color::Blue)
      << "\n";
  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CUserInterfaceEqualsMatcher detects UI visibility differences",
          "[unit][Components][CUserInterface][matcher]") {
  steamrot::CUserInterface expected;
  expected.m_active = true;
  expected.m_name = "test_ui";
  expected.m_visible = true;

  steamrot::CUserInterface actual;
  actual.m_active = true;
  actual.m_name = "test_ui";
  actual.m_visible = false;

  REQUIRE_THAT(actual, !steamrot::tests::EqualsCUserInterface(expected));
}
