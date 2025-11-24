/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CMetaEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CMetaEqualsMatcher.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CMetaEqualsMatcher works correctly",
          "[unit][Components][CMeta][matcher]") {
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;

  steamrot::CMeta actual;

  SECTION("Matcher detects differences") {
    actual.m_active = false;
    actual.m_entity_active = false;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCMeta(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_active = true;
    actual.m_entity_active = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCMeta(expected));
  }
}

TEST_CASE("CMetaEqualsMatcher describe is as expected on success",
          "[unit][Components][CMeta][matcher]") {
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;
  steamrot::CMeta actual;
  actual.m_active = true;
  actual.m_entity_active = true;
  auto matcher = steamrot::tests::EqualsCMeta(expected);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestPassed() + " Entity [?] CMeta Match:", 3) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CMetaEqualsMatcher describe is as expected on failure",
          "[unit][Components][CMeta][matcher]") {
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;
  steamrot::CMeta actual;
  actual.m_active = false;
  actual.m_entity_active = false;

  auto matcher = steamrot::tests::EqualsCMeta(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + " Entity [?] CMeta Match:", 3) << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed() << "m_active:"
      << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize(actual.m_active, conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize(expected.m_active, conmat::Color::Blue) << "\n";

  oss << conmat::Indent(1) << conmat::TestFailed() << "m_entity_active:"
      << "\n";
  oss << conmat::Indent(2) << "actual: "
      << conmat::Colorize(actual.m_entity_active, conmat::Color::Red) << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize(expected.m_entity_active, conmat::Color::Blue)
      << "\n";
  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CMetaEqualsMatcher with entity index shows index in output",
          "[unit][Components][CMeta][matcher]") {
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;
  steamrot::CMeta actual;
  actual.m_active = true;
  actual.m_entity_active = true;

  SECTION("Entity index 0") {
    auto matcher = steamrot::tests::EqualsCMeta(expected, 0);
    matcher.match(actual);

    std::string description = matcher.describe();
    REQUIRE(description.find("Entity [0]") != std::string::npos);
    REQUIRE(description.find("CMeta") != std::string::npos);
  }

  SECTION("Entity index 42") {
    auto matcher = steamrot::tests::EqualsCMeta(expected, 42);
    matcher.match(actual);

    std::string description = matcher.describe();
    REQUIRE(description.find("Entity [42]") != std::string::npos);
    REQUIRE(description.find("CMeta") != std::string::npos);
  }
}

TEST_CASE("CMetaEqualsMatcher without entity index shows unknown marker",
          "[unit][Components][CMeta][matcher]") {
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;
  steamrot::CMeta actual;
  actual.m_active = true;
  actual.m_entity_active = true;

  auto matcher = steamrot::tests::EqualsCMeta(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("Entity [?]") != std::string::npos);
  REQUIRE(description.find("CMeta") != std::string::npos);
}
