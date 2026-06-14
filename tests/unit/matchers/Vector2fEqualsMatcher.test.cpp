/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Vector2fEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Vector2fEqualsMatcher.h"
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Vector2fEqualsMatcher works correctly",
          "[unit][Vector2f][matcher]") {
  sf::Vector2f expected{10.0f, 20.0f};
  sf::Vector2f actual;

  SECTION("Matcher detects differences in x component") {
    actual.x = 15.0f;
    actual.y = 20.0f;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsVector2f(expected));
  }

  SECTION("Matcher detects differences in y component") {
    actual.x = 10.0f;
    actual.y = 25.0f;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsVector2f(expected));
  }

  SECTION("Matcher detects differences in both components") {
    actual.x = 15.0f;
    actual.y = 25.0f;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsVector2f(expected));
  }

  SECTION("Matcher detects equality") {
    actual.x = 10.0f;
    actual.y = 20.0f;
    REQUIRE_THAT(actual, steamrot::tests::EqualsVector2f(expected));
  }
}

TEST_CASE("Vector2fEqualsMatcher respects epsilon tolerance",
          "[unit][Vector2f][matcher]") {
  sf::Vector2f expected{10.0f, 20.0f};

  SECTION("Small difference within epsilon passes") {
    sf::Vector2f actual{10.000001f, 20.000001f};
    REQUIRE_THAT(actual, steamrot::tests::EqualsVector2f(expected));
  }

  SECTION("Large difference beyond epsilon fails") {
    sf::Vector2f actual{10.1f, 20.0f};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsVector2f(expected));
  }

  SECTION("Custom epsilon allows larger tolerance") {
    sf::Vector2f actual{10.05f, 20.05f};
    REQUIRE_THAT(actual, steamrot::tests::EqualsVector2f(expected, 0.1f));
  }

  SECTION("Custom epsilon still detects differences beyond tolerance") {
    sf::Vector2f actual{10.2f, 20.0f};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsVector2f(expected, 0.1f));
  }
}

TEST_CASE("Vector2fEqualsMatcher describe is as expected on success",
          "[unit][Vector2f][matcher]") {
  sf::Vector2f expected{10.0f, 20.0f};
  sf::Vector2f actual{10.0f, 20.0f};
  auto matcher = steamrot::tests::EqualsVector2f(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("sf::Vector2f Match:") != std::string::npos);
  REQUIRE(description.find("(10, 20)") != std::string::npos);
}

TEST_CASE("Vector2fEqualsMatcher describe is as expected on failure",
          "[unit][Vector2f][matcher]") {
  sf::Vector2f expected{10.0f, 20.0f};
  sf::Vector2f actual{15.0f, 25.0f};

  auto matcher = steamrot::tests::EqualsVector2f(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("sf::Vector2f Match:") != std::string::npos);
  REQUIRE(description.find("x component:") != std::string::npos);
  REQUIRE(description.find("y component:") != std::string::npos);
  REQUIRE(description.find("actual: ") != std::string::npos);
  REQUIRE(description.find("expected: ") != std::string::npos);
  REQUIRE(description.find("difference: ") != std::string::npos);
  REQUIRE(description.find("epsilon:") != std::string::npos);
}

TEST_CASE("Vector2fEqualsMatcher handles zero vectors",
          "[unit][Vector2f][matcher]") {
  sf::Vector2f zero{0.0f, 0.0f};

  SECTION("Zero equals zero") {
    sf::Vector2f actual{0.0f, 0.0f};
    REQUIRE_THAT(actual, steamrot::tests::EqualsVector2f(zero));
  }

  SECTION("Near-zero within epsilon passes") {
    sf::Vector2f actual{0.0000001f, 0.0000001f};
    REQUIRE_THAT(actual, steamrot::tests::EqualsVector2f(zero));
  }

  SECTION("Non-zero fails") {
    sf::Vector2f actual{0.1f, 0.1f};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsVector2f(zero));
  }
}

TEST_CASE("Vector2fEqualsMatcher handles negative values",
          "[unit][Vector2f][matcher]") {
  sf::Vector2f expected{-10.0f, -20.0f};

  SECTION("Exact negative match passes") {
    sf::Vector2f actual{-10.0f, -20.0f};
    REQUIRE_THAT(actual, steamrot::tests::EqualsVector2f(expected));
  }

  SECTION("Different negative values fail") {
    sf::Vector2f actual{-15.0f, -25.0f};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsVector2f(expected));
  }

  SECTION("Mixed positive/negative comparison") {
    sf::Vector2f actual{10.0f, 20.0f};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsVector2f(expected));
  }
}

TEST_CASE("Vector2fEqualsMatcher with only x component different",
          "[unit][Vector2f][matcher]") {
  sf::Vector2f expected{10.0f, 20.0f};
  sf::Vector2f actual{15.0f, 20.0f};

  auto matcher = steamrot::tests::EqualsVector2f(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("x component:") != std::string::npos);
  REQUIRE(description.find("y component:") == std::string::npos);
}

TEST_CASE("Vector2fEqualsMatcher with only y component different",
          "[unit][Vector2f][matcher]") {
  sf::Vector2f expected{10.0f, 20.0f};
  sf::Vector2f actual{10.0f, 25.0f};

  auto matcher = steamrot::tests::EqualsVector2f(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("x component:") == std::string::npos);
  REQUIRE(description.find("y component:") != std::string::npos);
}
