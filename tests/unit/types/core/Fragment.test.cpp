/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FailInfo type
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Fragment: Default constructor", "[types]") {
  steamrot::Fragment fragment;
  REQUIRE(fragment.name == "unnamed fragment");
  REQUIRE(fragment.sockets.empty());
  REQUIRE(fragment.render_overlays.empty());
  REQUIRE(fragment.transform == sf::Transform::Identity);
}
