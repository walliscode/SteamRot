/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the JointBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointBuilder.h"
#include "catch2/catch_approx.hpp"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {
using namespace parts;

TEST_CASE("JointBuilder tests", "[unit][part_library][JointBuilder]") {
  JointBuilder builder;

  SECTION("build() returns a Joint with the specified name") {
    const std::string joint_name = "TestJoint";
    Joint joint = builder.Build(joint_name);
    REQUIRE(joint.name == joint_name);
  }

  SECTION("AddViews() sets the positioning_views of the Joint") {
    const std::string joint_name = "TestJointWithViews";
    Joint joint = builder.AddViews(PremadeViews::WhiteSquare).Build(joint_name);
    REQUIRE(!joint.positioning_views.empty());
    REQUIRE(joint.positioning_views.size() == 1);
  }

  SECTION("SetOrigin() sets the origin of the Joint") {
    const std::string joint_name = "TestJointWithOrigin";
    sf::Vector2f origin(10.0f, 20.0f);
    Joint joint = builder.SetOrigin(origin).Build(joint_name);
    REQUIRE(joint.socket_pivot == origin);
  }

  SECTION("SetSocketConfig() sets the socket configuration of the Joint") {
    const std::string joint_name = "TestJointWithSocketConfig";
    Joint joint =
        builder.SetSocketConfig(4, 1.0f, 0.1f, 45.0f, true, 0.0f, 90.0f)
            .Build(joint_name);
    REQUIRE(joint.socket_config.socket_count == 4);
    REQUIRE(joint.socket_config.radius == Catch::Approx(1.0f));
    REQUIRE(joint.socket_config.minimum_gap == Catch::Approx(0.1f));
    REQUIRE(joint.socket_config.fixed_socket_angle == Catch::Approx(45.0f));
    REQUIRE(joint.socket_config.has_fixed_socket == true);
    REQUIRE(joint.socket_config.rotation_arc_min == Catch::Approx(0.0f));
    REQUIRE(joint.socket_config.rotation_arc_max == Catch::Approx(90.0f));
  }
  SECTION("Chaining AddViews() and SetSocketConfig() works correctly") {
    const std::string joint_name = "TestJointChained";
    Joint joint =
        builder.AddViews(PremadeViews::WhiteOblong)
            .SetSocketConfig(2, 0.5f, 0.05f, 30.0f, false, 0.0f, 60.0f)
            .Build(joint_name);
    REQUIRE(!joint.positioning_views.empty());
    REQUIRE(joint.positioning_views.size() == 1);
    REQUIRE(joint.socket_config.socket_count == 2);
    REQUIRE(joint.socket_config.radius == Catch::Approx(0.5f));
    REQUIRE(joint.socket_config.minimum_gap == Catch::Approx(0.05f));
    REQUIRE(joint.socket_config.fixed_socket_angle == Catch::Approx(30.0f));
    REQUIRE(joint.socket_config.has_fixed_socket == false);
    REQUIRE(joint.socket_config.rotation_arc_min == Catch::Approx(0.0f));
    REQUIRE(joint.socket_config.rotation_arc_max == Catch::Approx(60.0f));
  }
}
} // namespace steamrot::tests
