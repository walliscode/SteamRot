//////////////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the free functions for the spatial analysis of a grab
/// structure
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "grab_analysis.h"
#include "JointInstance.h"
#include "MachinaFormScaffold.h"
#include "PartGraphBuilder.h"
#include "SocketState.h"
#include "TerminalDescriptorFormatter.h"
#include "Vector2fEqualsMatcher.h"
#include "descriptors_machina_archetypes.h"
#include "joint_library.h"
#include "machina_archetype_packages.h"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <variant>

namespace steamrot::tests {
using namespace steamrot::logic;

TEST_CASE("valid_grab_pkg passes grab structural tests") {
  PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  MachinaArchetypeResult result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);

  // grab formatter to print the trace if the test fails
  descriptors::TerminalDescriptorFormatter fmt;
  if (!result)
    FAIL(fmt.Format(result.m_trace));
}

TEST_CASE("align_anchor_joint_to_anchor_point tests") {

  // ARRANGE //

  SECTION("ji_one tests") {

    JointInstance ji_one = JointInstance{0, parts::JointSquareWithOneSocket};
    ji_one.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);

    REQUIRE_THAT(ji_one.GetSocketWorldPosition(0),
                 EqualsVector2f({9.19f, 9.19f}, 0.01f));
    REQUIRE_THAT(ji_one.GetSocketPivotWorldPosition(),
                 EqualsVector2f({0.f, 0.f}, 0.01f));

    SECTION("anchor point at (0,0)") {

      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_one, {0.f, 0.f});

      // ASSERT //

      // the socket pivot should be at the anchor point
      REQUIRE_THAT(ji_one.GetSocketPivotWorldPosition(),
                   EqualsVector2f({0, 0}, 0.01f));
      // for 0-90 degrees rotation arc, the socket should be pointing straight
      // down
      REQUIRE_THAT(ji_one.GetSocketWorldPosition(0),
                   EqualsVector2f({0.f, 13.f}, 0.01f));
    }

    SECTION("anchor point at (10,10)") {
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_one,
                                                           {10.f, 10.f});
      // ASSERT //
      REQUIRE_THAT(ji_one.GetSocketPivotWorldPosition(),
                   EqualsVector2f({10.f, 10.f}, 0.01f));
      REQUIRE_THAT(ji_one.GetSocketWorldPosition(0),
                   EqualsVector2f({10.f, 23.f}, 0.01f));
    }

    SECTION("anchor point at (-10,-10)") {
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_one,
                                                           {-10.f, -10.f});
      // ASSERT //
      REQUIRE_THAT(ji_one.GetSocketPivotWorldPosition(),
                   EqualsVector2f({-10.f, -10.f}, 0.01f));
      REQUIRE_THAT(ji_one.GetSocketWorldPosition(0),
                   EqualsVector2f({-10.f, 3.f}, 0.01f));
    }

    SECTION("ji_two tests") {

      JointInstance ji_two = JointInstance{0, parts::JointSquareWithTwoSockets};
      ji_two.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);

      REQUIRE_THAT(ji_two.GetSocketWorldPosition(0),
                   EqualsVector2f({13, 0}, 0.01f));
      REQUIRE_THAT(ji_two.GetSocketWorldPosition(1),
                   EqualsVector2f({0, 13}, 0.01f));
      REQUIRE_THAT(ji_two.GetSocketPivotWorldPosition(),
                   EqualsVector2f({0.f, 0.f}, 0.01f));

      SECTION("anchor point at (0,0)") {
        // ACT //
        spatial_analysis::align_anchor_joint_to_anchor_point(ji_two,
                                                             {0.f, 0.f});
        // ASSERT //
        REQUIRE_THAT(ji_two.GetSocketPivotWorldPosition(),
                     EqualsVector2f({0.f, 0.f}, 0.01f));
        REQUIRE_THAT(ji_two.GetSocketWorldPosition(0),
                     EqualsVector2f({9.19f, 9.19f}, 0.01f));
        REQUIRE_THAT(ji_two.GetSocketWorldPosition(1),
                     EqualsVector2f({-9.19f, 9.19f}, 0.01f));
      }

      SECTION("anchor point at (10,10)") {
        // ACT //
        spatial_analysis::align_anchor_joint_to_anchor_point(ji_two,
                                                             {10.f, 10.f});
        // ASSERT //
        REQUIRE_THAT(ji_two.GetSocketPivotWorldPosition(),
                     EqualsVector2f({10.f, 10.f}, 0.01f));
        REQUIRE_THAT(ji_two.GetSocketWorldPosition(0),
                     EqualsVector2f({19.19f, 19.19f}, 0.01f));
        REQUIRE_THAT(ji_two.GetSocketWorldPosition(1),
                     EqualsVector2f({0.81f, 19.19f}, 0.01f));
      }

      SECTION("anchor point at (-10,-10)") {
        // ACT //
        spatial_analysis::align_anchor_joint_to_anchor_point(ji_two,
                                                             {-10.f, -10.f});
        // ASSERT //
        REQUIRE_THAT(ji_two.GetSocketPivotWorldPosition(),
                     EqualsVector2f({-10.f, -10.f}, 0.01f));
        REQUIRE_THAT(ji_two.GetSocketWorldPosition(0),
                     EqualsVector2f({-0.81f, -0.81f}, 0.01f));
        REQUIRE_THAT(ji_two.GetSocketWorldPosition(1),
                     EqualsVector2f({-19.19f, -0.81f}, 0.01f));
      }
    }
    SECTION("ji_three tests") {

      JointInstance ji_three =
          JointInstance{0, parts::JointSquareWithThreeSockets};
      ji_three.PositionSockets(
          JointSocketPositioningStrategy::MaximizeDistance);

      REQUIRE_THAT(ji_three.GetSocketWorldPosition(0),
                   EqualsVector2f({13.f, 0.f}, 0.01f));
      REQUIRE_THAT(ji_three.GetSocketWorldPosition(1),
                   EqualsVector2f({9.19f, 9.19f}, 0.01f));
      REQUIRE_THAT(ji_three.GetSocketWorldPosition(2),
                   EqualsVector2f({0, 13.f}, 0.01f));
      REQUIRE_THAT(ji_three.GetSocketPivotWorldPosition(),
                   EqualsVector2f({0.f, 0.f}, 0.01f));

      SECTION("anchor point at (0,0)") {

        // ACT //
        spatial_analysis::align_anchor_joint_to_anchor_point(ji_three,
                                                             {0.f, 0.f});
        // ASSERT //
        REQUIRE_THAT(ji_three.GetSocketPivotWorldPosition(),
                     EqualsVector2f({0.f, 0.f}, 0.01f));
        REQUIRE_THAT(ji_three.GetSocketWorldPosition(0),
                     EqualsVector2f({9.19f, 9.19f}, 0.01f));
        REQUIRE_THAT(ji_three.GetSocketWorldPosition(1),
                     EqualsVector2f({0.f, 13.f}, 0.01f));
        REQUIRE_THAT(ji_three.GetSocketWorldPosition(2),
                     EqualsVector2f({-9.19f, 9.19f}, 0.01f));
      }

      SECTION("anchor point at (10,10)") {

        // ACT //
        spatial_analysis::align_anchor_joint_to_anchor_point(ji_three,
                                                             {10.f, 10.f});
        // ASSERT //
        REQUIRE_THAT(ji_three.GetSocketPivotWorldPosition(),
                     EqualsVector2f({10.f, 10.f}, 0.01f));
        REQUIRE_THAT(ji_three.GetSocketWorldPosition(0),
                     EqualsVector2f({19.19f, 19.19f}, 0.01f));
        REQUIRE_THAT(ji_three.GetSocketWorldPosition(1),
                     EqualsVector2f({10.f, 23.f}, 0.01f));
        REQUIRE_THAT(ji_three.GetSocketWorldPosition(2),
                     EqualsVector2f({0.81f, 19.19f}, 0.01f));
      }
      SECTION("anchor point at (-10,-10)") {

        // ACT //
        spatial_analysis::align_anchor_joint_to_anchor_point(ji_three,
                                                             {-10.f, -10.f});
        // ASSERT //
        REQUIRE_THAT(ji_three.GetSocketPivotWorldPosition(),
                     EqualsVector2f({-10.f, -10.f}, 0.01f));
        REQUIRE_THAT(ji_three.GetSocketWorldPosition(0),
                     EqualsVector2f({-0.81f, -0.81f}, 0.01f));
        REQUIRE_THAT(ji_three.GetSocketWorldPosition(1),
                     EqualsVector2f({-10.f, 3.f}, 0.01f));
        REQUIRE_THAT(ji_three.GetSocketWorldPosition(2),
                     EqualsVector2f({-19.19f, -0.81f}, 0.01f));
      }
    }
  }
}

TEST_CASE("assign_left_and_right_arm_sockets tests") {}

TEST_CASE("align_grab_result_to_open_state tests") {
  // ARRANGE //
  PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  MachinaArchetypeResult ma_result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);
  REQUIRE(std::holds_alternative<GrabResult>(ma_result.result_sub_graphs));
  GrabResult grab_result = std::get<GrabResult>(ma_result.result_sub_graphs);
  PartGraph &graph = valid_grab_pkg.part_graph;

  JointInstance &anchor_joint =
      std::get<JointInstance>(graph.at(grab_result.anchor));

  // ACT //
  spatial_analysis::align_grab_result_to_open_state(grab_result, graph,
                                                    {0.f, 0.f});

  // ASSERT //

  // ANCHOR JOINT //
  REQUIRE_THAT(anchor_joint.GetSocketPivotWorldPosition(),
               EqualsVector2f({0.f, 0.f}, 0.01f));
  REQUIRE_THAT(anchor_joint.GetSocketWorldPosition(0),
               EqualsVector2f({9.19f, 9.19f}, 0.01f));
  REQUIRE_THAT(anchor_joint.GetSocketWorldPosition(1),
               EqualsVector2f({-9.19f, 9.19f}, 0.01f));

  // RIGHT ARM //
  const SubGraph &arm_one = grab_result.arms[0];

  /// RIGHT ARM: PART ONE ///
  const auto &arm_one_part_one = graph.at(arm_one[0]);
  REQUIRE(std::holds_alternative<FragmentInstance>(arm_one_part_one));
  const FragmentInstance &arm_one_part_one_fi =
      std::get<FragmentInstance>(arm_one_part_one);
  auto connection_one = anchor_joint.CheckForFirstConnectionWithOtherInstance(
      arm_one_part_one_fi);
  REQUIRE(connection_one.has_value());
  REQUIRE(connection_one->this_socket_id == 0);
  REQUIRE(connection_one->other_socket_id == 1);

  REQUIRE_THAT(arm_one_part_one_fi.GetSocketWorldPosition(0),
               EqualsVector2f({44.55f, 44.55f}, 0.01f));
  REQUIRE_THAT(arm_one_part_one_fi.GetSocketWorldPosition(1),
               EqualsVector2f({9.19f, 9.19f}, 0.01f));

  /// RIGHT ARM: PART TWO ///
  const auto &arm_one_part_two = graph.at(arm_one[1]);
  REQUIRE(std::holds_alternative<JointInstance>(arm_one_part_two));
  const JointInstance &arm_one_part_two_ji =
      std::get<JointInstance>(arm_one_part_two);
  auto connection_two =
      arm_one_part_one_fi.CheckForFirstConnectionWithOtherInstance(
          arm_one_part_two_ji);
  REQUIRE(connection_two.has_value());
  REQUIRE(connection_two->this_socket_id == 0);
  REQUIRE(connection_two->other_socket_id == 1);

  REQUIRE_THAT(arm_one_part_two_ji.GetSocketWorldPosition(1),
               EqualsVector2f({44.55f, 44.55f}, 0.01f));
  REQUIRE_THAT(arm_one_part_two_ji.GetSocketPivotWorldPosition(),
               EqualsVector2f({53.74f, 53.74f}, 0.01f));
  REQUIRE_THAT(arm_one_part_two_ji.GetSocketWorldPosition(0),
               EqualsVector2f({44.55f, 62.93f}, 0.01f));

  /// RIGHT ARM: PART THREE ///
  const auto &arm_one_part_three = graph.at(arm_one[2]);
  REQUIRE(std::holds_alternative<FragmentInstance>(arm_one_part_three));
  const FragmentInstance &arm_one_part_three_fi =
      std::get<FragmentInstance>(arm_one_part_three);
  auto connection_three =
      arm_one_part_two_ji.CheckForFirstConnectionWithOtherInstance(
          arm_one_part_three_fi);
  REQUIRE(connection_three.has_value());
  REQUIRE(connection_three->this_socket_id == 0);
  REQUIRE(connection_three->other_socket_id == 0);

  REQUIRE_THAT(arm_one_part_three_fi.GetSocketWorldPosition(0),
               EqualsVector2f({44.55f, 62.93f}, 0.01f));
  REQUIRE_THAT(arm_one_part_three_fi.GetSocketWorldPosition(1),
               EqualsVector2f({9.19f, 98.28f}, 0.01f));

  // LEFT ARM //
  const SubGraph &arm_two = grab_result.arms[1];

  /// LEFT ARM: PART ONE ///
  const auto &arm_two_part_one = graph.at(arm_two[0]);
  REQUIRE(std::holds_alternative<FragmentInstance>(arm_two_part_one));
  const FragmentInstance &arm_two_part_one_fi =
      std::get<FragmentInstance>(arm_two_part_one);
  auto arm_two_connection_one =
      anchor_joint.CheckForFirstConnectionWithOtherInstance(
          arm_two_part_one_fi);
  REQUIRE(arm_two_connection_one.has_value());
  REQUIRE(arm_two_connection_one->this_socket_id == 1);
  REQUIRE(arm_two_connection_one->other_socket_id == 0);

  REQUIRE_THAT(arm_two_part_one_fi.GetSocketWorldPosition(0),
               EqualsVector2f({-9.19f, 9.19f}, 0.01f));
  REQUIRE_THAT(arm_two_part_one_fi.GetSocketWorldPosition(1),
               EqualsVector2f({-44.55f, 44.55f}, 0.01f));

  /// LEFT ARM: PART TWO ///
  const auto &arm_two_part_two = graph.at(arm_two[1]);
  REQUIRE(std::holds_alternative<JointInstance>(arm_two_part_two));
  const JointInstance &arm_two_part_two_ji =
      std::get<JointInstance>(arm_two_part_two);

  auto arm_two_connection_two =
      arm_two_part_one_fi.CheckForFirstConnectionWithOtherInstance(
          arm_two_part_two_ji);
  REQUIRE(arm_two_connection_two.has_value());
  REQUIRE(arm_two_connection_two->this_socket_id == 1);
  REQUIRE(arm_two_connection_two->other_socket_id == 0);

  REQUIRE_THAT(arm_two_part_two_ji.GetSocketWorldPosition(0),
               EqualsVector2f({-44.55f, 44.55f}, 0.01f));
  REQUIRE_THAT(arm_two_part_two_ji.GetSocketPivotWorldPosition(),
               EqualsVector2f({-53.74f, 53.74f}, 0.01f));
  REQUIRE_THAT(arm_two_part_two_ji.GetSocketWorldPosition(1),
               EqualsVector2f({-44.55f, 62.93f}, 0.01f));

  /// LEFT ARM: PART THREE ///
  const auto &arm_two_part_three = graph.at(arm_two[2]);
  REQUIRE(std::holds_alternative<FragmentInstance>(arm_two_part_three));
  const FragmentInstance &arm_two_part_three_fi =
      std::get<FragmentInstance>(arm_two_part_three);

  auto arm_two_connection_three =
      arm_two_part_two_ji.CheckForFirstConnectionWithOtherInstance(
          arm_two_part_three_fi);
  REQUIRE(arm_two_connection_three.has_value());
  REQUIRE(arm_two_connection_three->this_socket_id == 1);
  REQUIRE(arm_two_connection_three->other_socket_id == 0);

  REQUIRE_THAT(arm_two_part_three_fi.GetSocketWorldPosition(0),
               EqualsVector2f({-44.55f, 62.93f}, 0.01f));
  REQUIRE_THAT(arm_two_part_three_fi.GetSocketWorldPosition(1),
               EqualsVector2f({-9.19f, 98.28f}, 0.01f));
}

TEST_CASE("end_of_arm_is_grab_ready tests") {

  // ARRANGE //
  PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  MachinaArchetypeResult ma_result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);

  REQUIRE(std::holds_alternative<GrabResult>(ma_result.result_sub_graphs));
  GrabResult grab_result = std::get<GrabResult>(ma_result.result_sub_graphs);
  PartGraph &graph = valid_grab_pkg.part_graph;

  JointInstance &anchor_joint =
      std::get<JointInstance>(graph.at(grab_result.anchor));

  spatial_analysis::align_anchor_joint_to_anchor_point(anchor_joint,
                                                       {0.f, 0.f});

  SECTION("if Subgraph is empty, return false") {
    SubGraph arm{};
    REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
        arm, false, anchor_joint, valid_grab_pkg.part_graph));
  }

  SECTION("if PartGraph is empty, return false") {
    SubGraph arm{1, 2, 3};
    PartGraph empty_graph{};
    REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
        arm, false, anchor_joint, empty_graph));
  }

  SECTION("Left arm tests") {
    const SubGraph &left_arm = grab_result.arms[1];
    auto &left_arm_end_part = graph.at(left_arm.back());
    REQUIRE(std::holds_alternative<FragmentInstance>(left_arm_end_part));

    FragmentInstance &left_arm_end_fi =
        std::get<FragmentInstance>(left_arm_end_part);

    REQUIRE_THAT(left_arm_end_fi.GetSockets()
                     .at(0)
                     .GetLocalAlignmentVector()
                     .normalized(),
                 Vector2fEqualsMatcher({1.f, 0.f}, 0.01f));

    SECTION("rotation 0 degrees") {
      left_arm_end_fi.setRotation(sf::degrees(0.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 45 degrees") {
      left_arm_end_fi.setRotation(sf::degrees(45.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 90 degrees") {
      left_arm_end_fi.setRotation(sf::degrees(90.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 135 degrees") {
      left_arm_end_fi.setRotation(sf::degrees(135.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 180 degrees") {
      left_arm_end_fi.setRotation(sf::degrees(180.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 225 degrees") {
      left_arm_end_fi.setRotation(sf::degrees(225.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 270 degrees") {
      left_arm_end_fi.setRotation(sf::degrees(270.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }
  }

  SECTION("Right arm tests") {
    const SubGraph &right_arm = grab_result.arms[0];
    auto &right_arm_end_part = graph.at(right_arm.back());
    REQUIRE(std::holds_alternative<FragmentInstance>(right_arm_end_part));
    FragmentInstance &right_arm_end_fi =
        std::get<FragmentInstance>(right_arm_end_part);

    REQUIRE_THAT(right_arm_end_fi.GetSockets()
                     .at(0)
                     .GetLocalAlignmentVector()
                     .normalized(),
                 Vector2fEqualsMatcher({1.f, 0.f}, 0.01f));

    SECTION("rotation 0 degrees") {
      right_arm_end_fi.setRotation(sf::degrees(0.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 45 degrees") {
      right_arm_end_fi.setRotation(sf::degrees(45.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 90 degrees") {
      right_arm_end_fi.setRotation(sf::degrees(90.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 135 degrees") {
      right_arm_end_fi.setRotation(sf::degrees(135.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 180 degrees") {
      right_arm_end_fi.setRotation(sf::degrees(180.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 225 degrees") {
      right_arm_end_fi.setRotation(sf::degrees(225.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 270 degrees") {
      right_arm_end_fi.setRotation(sf::degrees(270.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }
  }
}

TEST_CASE("all_arms_are_grab_ready tests") {
  PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  MachinaArchetypeResult ma_result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);
  REQUIRE(std::holds_alternative<GrabResult>(ma_result.result_sub_graphs));
  GrabResult grab_result = std::get<GrabResult>(ma_result.result_sub_graphs);
  PartGraph &graph = valid_grab_pkg.part_graph;

  JointInstance &anchor_joint =
      std::get<JointInstance>(graph.at(grab_result.anchor));

  spatial_analysis::align_anchor_joint_to_anchor_point(anchor_joint,
                                                       {0.f, 0.f});

  const SubGraph &left_arm = grab_result.arms[1];
  auto &left_arm_end_part = graph.at(left_arm.back());

  const SubGraph &right_arm = grab_result.arms[0];
  auto &right_arm_end_part = graph.at(right_arm.back());
  REQUIRE(std::holds_alternative<FragmentInstance>(right_arm_end_part));
  FragmentInstance &right_arm_end_fi =
      std::get<FragmentInstance>(right_arm_end_part);
  REQUIRE(std::holds_alternative<FragmentInstance>(left_arm_end_part));
  FragmentInstance &left_arm_end_fi =
      std::get<FragmentInstance>(left_arm_end_part);

  SECTION("both arms are grab ready") {
    left_arm_end_fi.setRotation(sf::degrees(90.f));
    right_arm_end_fi.setRotation(sf::degrees(90.f));
    REQUIRE(spatial_analysis::all_arms_are_grab_ready(
        grab_result, valid_grab_pkg.part_graph));
  }
  SECTION("left arm is not grab ready") {
    left_arm_end_fi.setRotation(sf::degrees(135.f));
    REQUIRE_FALSE(spatial_analysis::all_arms_are_grab_ready(
        grab_result, valid_grab_pkg.part_graph));
  }
  SECTION("right arm is not grab ready") {
    right_arm_end_fi.setRotation(sf::degrees(45.f));
    REQUIRE_FALSE(spatial_analysis::all_arms_are_grab_ready(
        grab_result, valid_grab_pkg.part_graph));
  }
}
} // namespace steamrot::tests
