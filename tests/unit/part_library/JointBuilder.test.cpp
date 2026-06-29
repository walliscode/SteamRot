/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the JointBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointBuilder.h"
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
}
} // namespace steamrot::tests
