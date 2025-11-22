/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIElementEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIElementEqualsMatcher.h"
#include "ButtonElement.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIElementEqualsMatcher works correctly with ButtonElement",
          "[unit][UIElement][ButtonElement][matcher]") {
  steamrot::ButtonElement expected;
  expected.position = sf::Vector2f(100.0f, 200.0f);
  expected.size = sf::Vector2f(50.0f, 30.0f);
  expected.children_active = true;
  expected.label = "Test";

  steamrot::ButtonElement actual;

  SECTION("Matcher detects differences in position") {
    actual.position = sf::Vector2f(150.0f, 250.0f);
    actual.size = sf::Vector2f(50.0f, 30.0f);
    actual.children_active = true;
    actual.label = "Test";
    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects differences in size") {
    actual.position = sf::Vector2f(100.0f, 200.0f);
    actual.size = sf::Vector2f(60.0f, 40.0f);
    actual.children_active = true;
    actual.label = "Test";
    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects equality") {
    actual.position = sf::Vector2f(100.0f, 200.0f);
    actual.size = sf::Vector2f(50.0f, 30.0f);
    actual.children_active = true;
    actual.label = "Test";
    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }
}

TEST_CASE("UIElementEqualsMatcher describe is as expected on success",
          "[unit][UIElement][matcher]") {
  steamrot::ButtonElement expected;
  expected.position = sf::Vector2f(100.0f, 200.0f);
  steamrot::ButtonElement actual;
  actual.position = sf::Vector2f(100.0f, 200.0f);
  auto matcher = steamrot::tests::EqualsUIElement(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Divider("-", 40) << "\n";
  oss << conmat::TestPassed() << "UIElement Match" << "\n";
  oss << conmat::Divider("-", 40) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("UIElementEqualsMatcher describe is as expected on failure",
          "[unit][UIElement][matcher]") {
  steamrot::ButtonElement expected;
  expected.position = sf::Vector2f(100.0f, 200.0f);
  expected.size = sf::Vector2f(50.0f, 30.0f);

  steamrot::ButtonElement actual;
  actual.position = sf::Vector2f(150.0f, 250.0f);
  actual.size = sf::Vector2f(60.0f, 40.0f);

  auto matcher = steamrot::tests::EqualsUIElement(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("UIElement Match:") != std::string::npos);
  REQUIRE(description.find("position:") != std::string::npos);
  REQUIRE(description.find("size:") != std::string::npos);
}

TEST_CASE("UIElementEqualsMatcher works with different concrete types",
          "[unit][UIElement][ButtonElement][matcher]") {
  steamrot::ButtonElement expected;
  expected.position = sf::Vector2f(100.0f, 200.0f);
  expected.label = "Test Button";

  steamrot::ButtonElement actual;

  SECTION("Matcher detects differences in label") {
    actual.position = sf::Vector2f(100.0f, 200.0f);
    actual.label = "Different Label";
    REQUIRE_THAT(actual, !steamrot::tests::EqualsUIElement(expected));
  }

  SECTION("Matcher detects equality") {
    actual.position = sf::Vector2f(100.0f, 200.0f);
    actual.label = "Test Button";
    REQUIRE_THAT(actual, steamrot::tests::EqualsUIElement(expected));
  }
}
