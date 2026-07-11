/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for TransformEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TransformEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("TransformEqualsMatcher tests", "[unit][Transform][matcher]") {
  sf::Transform expected{sf::Transform::Identity};
  sf::Transform actual;

  SECTION("Compare constructed 3x3 matrices") {
    sf::Transform t1(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);

    sf::Transform t2(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);

    REQUIRE_THAT(t1, TransformEquals(t2));
  }
  SECTION("Exact match passes") {
    actual = expected;
    REQUIRE_THAT(actual, TransformEquals(expected));
  }

  SECTION("Exact match with non identity transform passes") {
    actual.translate({10.f, 20.f});
    expected.translate({10.f, 20.f});
    REQUIRE_THAT(actual, TransformEquals(expected));
  }

  SECTION("Different transform fails") {
    actual.translate({10.f, 20.f});
    REQUIRE_THAT(actual, !TransformEquals(expected));
  }

  SECTION("Near-equal transform within epsilon passes") {
    actual.translate({0.000001f, 0.000001f});
    REQUIRE_THAT(actual, TransformEquals(expected));
  }

  SECTION("Near-equal transform beyond epsilon fails") {
    actual.translate({0.1f, 0.1f});
    REQUIRE_THAT(actual, !TransformEquals(expected));
  }
}
} // namespace steamrot::tests
