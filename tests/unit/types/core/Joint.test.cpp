/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Joint struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Joint.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Joint struct initialization", "[types]") {
  steamrot::Joint joint;
  REQUIRE(joint.name == "unnamed");
  REQUIRE(joint.connection_number == 0);
  REQUIRE(joint.global_position == sf::Vector2f(0.f, 0.f));
  REQUIRE(joint.connected_fragments.empty());
  REQUIRE(joint.render_overlay.getVertexCount() == 0);
  REQUIRE(joint.transform == sf::Transform::Identity);
}
