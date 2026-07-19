/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the joint library
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "joint_library.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {
using namespace parts;

TEST_CASE(
    "CreateWhiteRectangleJointWith2Sockets creates a joint with 2 sockets",
    "[joint_library]") {
  const Joint &joint = JointSquareWithTwoSockets;

  REQUIRE(joint.name == "JointSquareWith2Sockets");
  REQUIRE(joint.socket_pivot == sf::Vector2f(10, 10));
  REQUIRE(joint.positioning_views.size() == 1);
  REQUIRE(joint.socket_config.socket_count == 2);
  REQUIRE(joint.socket_config.radius == 13);
  REQUIRE(joint.socket_config.minimum_gap == 0);
  REQUIRE(joint.socket_config.fixed_socket_angle == 0);
  REQUIRE(joint.socket_config.has_fixed_socket == false);
  REQUIRE(joint.socket_config.rotation_arc_min == 0);
  REQUIRE(joint.socket_config.rotation_arc_max == 90);
}
} // namespace steamrot::tests
