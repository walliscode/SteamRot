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
#include "Vector2fEqualsMatcher.h"
#include "action_grimoire_machina.h"
#include "catch2/catch_approx.hpp"
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

  REQUIRE(result);
}

TEST_CASE("align_anchor_joint_to_anchor_point tests") {

  // ARRANGE //
  JointInstance ji_one = JointInstance{0, parts::JointSquareWithOneSocket};
  ji_one.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
  const sf::Vector2f ji_one_socket_0_local_position =
      ji_one.GetSocketLocalPosition(0);

  REQUIRE(ji_one.GetTransform() == sf::Transform::Identity);
  REQUIRE_THAT(ji_one_socket_0_local_position,
               Vector2fEqualsMatcher({19.19f, 19.19f}, 0.01f));
  REQUIRE_THAT(ji_one.GetSocketWorldPosition(0),
               Vector2fEqualsMatcher({19.19f, 19.19f}, 0.01f));

  JointInstance ji_two = JointInstance{0, parts::JointSquareWithTwoSockets};
  ji_two.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
  const sf::Vector2f ji_two_socket_0_local_position =
      ji_two.GetSockets().at(0).GetLocalPosition();
  const sf::Vector2f ji_two_socket_1_local_position =
      ji_two.GetSockets().at(1).GetLocalPosition();
  REQUIRE_THAT(ji_two_socket_0_local_position,
               Vector2fEqualsMatcher({23.0f, 10.0f}));
  REQUIRE_THAT(ji_two_socket_1_local_position,
               Vector2fEqualsMatcher({10.0f, 23.0f}));

  JointInstance ji_three = JointInstance{0, parts::JointSquareWithThreeSockets};
  ji_three.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
  const sf::Vector2f ji_three_socket_0_local_position =
      ji_three.GetSockets().at(0).GetLocalPosition();
  const sf::Vector2f ji_three_socket_1_local_position =
      ji_three.GetSockets().at(1).GetLocalPosition();
  const sf::Vector2f ji_three_socket_2_local_position =
      ji_three.GetSockets().at(2).GetLocalPosition();
  REQUIRE_THAT(ji_three_socket_0_local_position,
               Vector2fEqualsMatcher({23.0f, 10.0f}));
  REQUIRE_THAT(ji_three_socket_1_local_position,
               Vector2fEqualsMatcher({19.19f, 19.19f}, 0.01f));
  REQUIRE_THAT(ji_three_socket_2_local_position,
               Vector2fEqualsMatcher({10.0f, 23.0f}));

  SECTION("ji_one tests") {
    SECTION("anchor point at (0,0)") {
      // ARRANGE //
      sf::Vector2f expected_socket_0_position{0.f, 13.f};

      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_one, {0.f, 0.f});

      // ASSERT //
      REQUIRE_THAT(
          ji_one.GetTransform().transformPoint(ji_one_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
    }

    SECTION("anchor point at (10,10)") {
      // ARRANGE //
      sf::Vector2f expected_socket_0_position{10.f, 23.f};
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_one,
                                                           {10.f, 10.f});
      // ASSERT //
      REQUIRE_THAT(
          ji_one.GetTransform().transformPoint(ji_one_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
    }

    SECTION("anchor point at (-10,-10)") {
      // ARRANGE //
      sf::Vector2f expected_socket_0_position{-10.f, 3.f};
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_one,
                                                           {-10.f, -10.f});
      // ASSERT //
      REQUIRE_THAT(
          ji_one.GetTransform().transformPoint(ji_one_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
    }
  }

  SECTION("ji_two tests") {
    SECTION("anchor point at (0,0)") {
      // ARRANGE //
      sf::Vector2f expected_socket_0_position{9.19f, 9.19f};
      sf::Vector2f expected_socket_1_position{-9.19f, 9.19f};
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_two, {0.f, 0.f});
      // ASSERT //
      REQUIRE_THAT(
          ji_two.GetTransform().transformPoint(ji_two_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(
          ji_two.GetTransform().transformPoint(ji_two_socket_1_local_position),
          Vector2fEqualsMatcher(expected_socket_1_position, 0.01f));
    }

    SECTION("anchor point at (10,10)") {
      // ARRANGE //
      sf::Vector2f expected_socket_0_position{19.19f, 19.19f};
      sf::Vector2f expected_socket_1_position{0.808f, 19.19f};
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_two,
                                                           {10.f, 10.f});
      // ASSERT //
      REQUIRE_THAT(
          ji_two.GetTransform().transformPoint(ji_two_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(
          ji_two.GetTransform().transformPoint(ji_two_socket_1_local_position),
          Vector2fEqualsMatcher(expected_socket_1_position, 0.01f));
    }
    SECTION("anchor point at (-10,-10)") {
      // ARRANGE //
      sf::Vector2f expected_socket_0_position{-0.81f, -0.81f};
      sf::Vector2f expected_socket_1_position{-19.19f, -0.81f};
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_two,
                                                           {-10.f, -10.f});
      // ASSERT //
      REQUIRE_THAT(
          ji_two.GetTransform().transformPoint(ji_two_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(
          ji_two.GetTransform().transformPoint(ji_two_socket_1_local_position),
          Vector2fEqualsMatcher(expected_socket_1_position, 0.01f));
    }
  }
  SECTION("ji_three tests") {
    SECTION("anchor point at (0,0)") {
      // ARRANGE //
      sf::Vector2f expected_socket_0_position{9.19f, 9.19f};
      sf::Vector2f expected_socket_1_position{0.f, 13.f};
      sf::Vector2f expected_socket_2_position{-9.19f, 9.19f};
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_three,
                                                           {0.f, 0.f});
      // ASSERT //
      REQUIRE_THAT(ji_three.GetTransform().transformPoint(
                       ji_three_socket_0_local_position),
                   Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(ji_three.GetTransform().transformPoint(
                       ji_three_socket_1_local_position),
                   Vector2fEqualsMatcher(expected_socket_1_position, 0.01f));
      REQUIRE_THAT(ji_three.GetTransform().transformPoint(
                       ji_three_socket_2_local_position),
                   Vector2fEqualsMatcher(expected_socket_2_position, 0.01f));
    }

    SECTION("anchor point at (10,10)") {
      // ARRANGE //
      sf::Vector2f expected_socket_0_position{19.19f, 19.19f};
      sf::Vector2f expected_socket_1_position{10.f, 23.f};
      sf::Vector2f expected_socket_2_position{0.81f, 19.19f};
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_three,
                                                           {10.f, 10.f});
      // ASSERT //
      REQUIRE_THAT(ji_three.GetTransform().transformPoint(
                       ji_three_socket_0_local_position),
                   Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(ji_three.GetTransform().transformPoint(
                       ji_three_socket_1_local_position),
                   Vector2fEqualsMatcher(expected_socket_1_position, 0.01f));
      REQUIRE_THAT(ji_three.GetTransform().transformPoint(
                       ji_three_socket_2_local_position),
                   Vector2fEqualsMatcher(expected_socket_2_position, 0.01f));
    }
    SECTION("anchor point at (-10,-10)") {
      // ARRANGE //
      sf::Vector2f expected_socket_0_position{-0.81f, -0.81f};
      sf::Vector2f expected_socket_1_position{-10.f, 3.f};
      sf::Vector2f expected_socket_2_position{-19.19f, -0.81f};
      // ACT //
      spatial_analysis::align_anchor_joint_to_anchor_point(ji_three,
                                                           {-10.f, -10.f});
      // ASSERT //
      REQUIRE_THAT(ji_three.GetTransform().transformPoint(
                       ji_three_socket_0_local_position),
                   Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(ji_three.GetTransform().transformPoint(
                       ji_three_socket_1_local_position),
                   Vector2fEqualsMatcher(expected_socket_1_position, 0.01f));
      REQUIRE_THAT(ji_three.GetTransform().transformPoint(
                       ji_three_socket_2_local_position),
                   Vector2fEqualsMatcher(expected_socket_2_position, 0.01f));
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
  sf::Vector2f expected_anchor_joint_socket_pivot_position{0.f, 0.f};
  REQUIRE_THAT(anchor_joint.GetSocketPivotWorldPosition(),
               Vector2fEqualsMatcher(
                   expected_anchor_joint_socket_pivot_position, 0.01f));
  sf::Vector2f expected_anchor_joint_socket_0_position{9.19f, 9.19f};
  REQUIRE_THAT(
      anchor_joint.GetTransform().transformPoint(
          anchor_joint.GetSockets().at(0).GetLocalPosition()),
      Vector2fEqualsMatcher(expected_anchor_joint_socket_0_position, 0.01f));
  sf::Vector2f expected_anchor_joint_socket_1_position{-9.19f, 9.19f};
  REQUIRE_THAT(
      anchor_joint.GetTransform().transformPoint(
          anchor_joint.GetSockets().at(1).GetLocalPosition()),
      Vector2fEqualsMatcher(expected_anchor_joint_socket_1_position, 0.01f));

  // RIGHT ARM //
  const SubGraph &arm_one = grab_result.arms[0];

  /// RIGHT ARM: PART ONE ///
  const auto &arm_one_part_one = graph.at(arm_one[0]);
  REQUIRE(std::holds_alternative<FragmentInstance>(arm_one_part_one));
  const FragmentInstance &arm_one_part_one_fi =
      std::get<FragmentInstance>(arm_one_part_one);
  auto connection_one = action::grimoire_machina::check_for_connected_sockets(
      anchor_joint, arm_one_part_one_fi);
  REQUIRE(connection_one.has_value());
  REQUIRE(connection_one->joint_socket_id == 0);
  REQUIRE(connection_one->fragment_socket_id == 1);
  REQUIRE(arm_one_part_one_fi.GetTotalRotation().asDegrees() == -135.f);

  sf::Vector2f expected_arm_one_part_one_socket_1_position{9.19f, 9.19f};
  REQUIRE_THAT(arm_one_part_one_fi.GetTransform().transformPoint(
                   arm_one_part_one_fi.GetSockets().at(1).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_one_socket_1_position, 0.01f));

  sf::Vector2f expected_arm_one_part_one_socket_0_position{44.55f, 44.55f};
  REQUIRE_THAT(arm_one_part_one_fi.GetTransform().transformPoint(
                   arm_one_part_one_fi.GetSockets().at(0).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_one_socket_0_position, 0.01f));

  // RIGHT ARM:: PART TWO ///
  const auto &arm_one_part_two = graph.at(arm_one[1]);
  REQUIRE(std::holds_alternative<JointInstance>(arm_one_part_two));
  const JointInstance &arm_one_part_two_ji =
      std::get<JointInstance>(arm_one_part_two);
  auto connection_two = action::grimoire_machina::check_for_connected_sockets(
      arm_one_part_two_ji, arm_one_part_one_fi);
  REQUIRE(connection_two.has_value());
  REQUIRE(connection_two->joint_socket_id == 1);
  REQUIRE(connection_two->fragment_socket_id == 0);

  sf::Vector2f expected_arm_one_part_two_socket_1_position{44.55f, 44.55f};
  REQUIRE_THAT(arm_one_part_two_ji.GetTransform().transformPoint(
                   arm_one_part_two_ji.GetSockets().at(1).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_two_socket_1_position, 0.01f));

  sf::Vector2f expected_arm_one_part_two_socket_pivot_position{53.74f, 53.74f};
  REQUIRE_THAT(arm_one_part_two_ji.GetSocketPivotWorldPosition(),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_two_socket_pivot_position, 0.01f));

  sf::Vector2f expected_arm_one_part_two_socket_0_position{44.55f, 62.93f};
  REQUIRE_THAT(arm_one_part_two_ji.GetTransform().transformPoint(
                   arm_one_part_two_ji.GetSockets().at(0).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_two_socket_0_position, 0.01f));

  /// RIGHT ARM: PART THREE ///
  const auto &arm_one_part_three = graph.at(arm_one[2]);
  REQUIRE(std::holds_alternative<FragmentInstance>(arm_one_part_three));
  const FragmentInstance &arm_one_part_three_fi =
      std::get<FragmentInstance>(arm_one_part_three);
  auto connection_three = action::grimoire_machina::check_for_connected_sockets(
      arm_one_part_two_ji, arm_one_part_three_fi);
  REQUIRE(connection_three.has_value());
  REQUIRE(connection_three->joint_socket_id == 0);
  REQUIRE(connection_three->fragment_socket_id == 0);
  REQUIRE(arm_one_part_three_fi.GetTotalRotation().asDegrees() == 135.f);

  sf::Vector2f expected_arm_one_part_three_socket_0_position{44.55f, 62.93f};
  REQUIRE_THAT(arm_one_part_three_fi.GetTransform().transformPoint(
                   arm_one_part_three_fi.GetSockets().at(0).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_three_socket_0_position, 0.01f));

  sf::Vector2f expected_arm_one_part_three_socket_1_position{9.19f, 98.28f};
  REQUIRE_THAT(arm_one_part_three_fi.GetTransform().transformPoint(
                   arm_one_part_three_fi.GetSockets().at(1).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_three_socket_1_position, 0.01f));

  // LEFT ARM //
  const SubGraph &arm_two = grab_result.arms[1];

  const auto &arm_two_part_one = graph.at(arm_two[0]);
  REQUIRE(std::holds_alternative<FragmentInstance>(arm_two_part_one));
  const FragmentInstance &arm_two_part_one_fi =
      std::get<FragmentInstance>(arm_two_part_one);
  auto arm_two_connection_two =
      action::grimoire_machina::check_for_connected_sockets(
          anchor_joint, arm_two_part_one_fi);
  REQUIRE(arm_two_connection_two.has_value());
  REQUIRE(arm_two_connection_two->joint_socket_id == 1);
  REQUIRE(arm_two_connection_two->fragment_socket_id == 0);
  REQUIRE(arm_two_part_one_fi.GetTotalRotation().asDegrees() == 135.f);

  sf::Vector2f expected_arm_two_part_one_socket_0_position{-9.19f, 9.19f};
  REQUIRE_THAT(arm_two_part_one_fi.GetTransform().transformPoint(
                   arm_two_part_one_fi.GetSockets().at(0).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_two_part_one_socket_0_position, 0.01f));
  sf::Vector2f expected_arm_two_part_one_socket_1_position{-44.55f, 44.55f};
  REQUIRE_THAT(arm_two_part_one_fi.GetTransform().transformPoint(
                   arm_two_part_one_fi.GetSockets().at(1).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_two_part_one_socket_1_position, 0.01f));

  const auto &arm_two_part_two = graph.at(arm_two[1]);
  REQUIRE(std::holds_alternative<JointInstance>(arm_two_part_two));
  const JointInstance &arm_two_part_two_ji =
      std::get<JointInstance>(arm_two_part_two);
  auto arm_two_connection_three =
      action::grimoire_machina::check_for_connected_sockets(
          arm_two_part_two_ji, arm_two_part_one_fi);
  REQUIRE(arm_two_connection_three.has_value());
  REQUIRE(arm_two_connection_three->joint_socket_id == 0);
  REQUIRE(arm_two_connection_three->fragment_socket_id == 1);
  REQUIRE(Catch::Approx(arm_two_part_two_ji.GetTotalRotation().asDegrees()) ==
          -45.f);

  sf::Vector2f expected_arm_two_part_two_socket_0_position{-44.55f, 44.55f};
  REQUIRE_THAT(arm_two_part_two_ji.GetTransform().transformPoint(
                   arm_two_part_two_ji.GetSockets().at(0).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_two_part_two_socket_0_position, 0.01f));
  sf::Vector2f expected_arm_two_part_two_socket_pivot_position{-53.74f, 53.74f};
  REQUIRE_THAT(arm_two_part_two_ji.GetSocketPivotWorldPosition(),
               Vector2fEqualsMatcher(
                   expected_arm_two_part_two_socket_pivot_position, 0.01f));
  sf::Vector2f expected_arm_two_part_two_socket_1_position{-44.55f, 62.93f};
  REQUIRE_THAT(arm_two_part_two_ji.GetTransform().transformPoint(
                   arm_two_part_two_ji.GetSockets().at(1).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_two_part_two_socket_1_position, 0.01f));

  const auto &arm_two_part_three = graph.at(arm_two[2]);
  REQUIRE(std::holds_alternative<FragmentInstance>(arm_two_part_three));
  const FragmentInstance &arm_two_part_three_fi =
      std::get<FragmentInstance>(arm_two_part_three);
  auto arm_two_connection_four =
      action::grimoire_machina::check_for_connected_sockets(
          arm_two_part_two_ji, arm_two_part_three_fi);
  REQUIRE(arm_two_connection_four.has_value());
  REQUIRE(arm_two_connection_four->joint_socket_id == 1);
  REQUIRE(arm_two_connection_four->fragment_socket_id == 0);
  REQUIRE(Catch::Approx(arm_two_part_three_fi.GetTotalRotation().asDegrees()) ==
          45.f);

  sf::Vector2f expected_arm_two_part_three_socket_0_position{-44.55f, 62.93f};
  REQUIRE_THAT(arm_two_part_three_fi.GetTransform().transformPoint(
                   arm_two_part_three_fi.GetSockets().at(0).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_two_part_three_socket_0_position, 0.01f));
  sf::Vector2f expected_arm_two_part_three_socket_1_position{-9.19f, 98.28f};
  REQUIRE_THAT(arm_two_part_three_fi.GetTransform().transformPoint(
                   arm_two_part_three_fi.GetSockets().at(1).GetLocalPosition()),
               Vector2fEqualsMatcher(
                   expected_arm_two_part_three_socket_1_position, 0.01f));
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
      left_arm_end_fi.SetTotalRotation(sf::degrees(0.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 45 degrees") {
      left_arm_end_fi.SetTotalRotation(sf::degrees(45.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 90 degrees") {
      left_arm_end_fi.SetTotalRotation(sf::degrees(90.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 135 degrees") {
      left_arm_end_fi.SetTotalRotation(sf::degrees(135.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 180 degrees") {
      left_arm_end_fi.SetTotalRotation(sf::degrees(180.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 225 degrees") {
      left_arm_end_fi.SetTotalRotation(sf::degrees(225.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          left_arm, true, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 270 degrees") {
      left_arm_end_fi.SetTotalRotation(sf::degrees(270.f));
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
      right_arm_end_fi.SetTotalRotation(sf::degrees(0.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 45 degrees") {
      right_arm_end_fi.SetTotalRotation(sf::degrees(45.f));
      REQUIRE_FALSE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 90 degrees") {
      right_arm_end_fi.SetTotalRotation(sf::degrees(90.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }

    SECTION("rotation 135 degrees") {
      right_arm_end_fi.SetTotalRotation(sf::degrees(135.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 180 degrees") {
      right_arm_end_fi.SetTotalRotation(sf::degrees(180.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 225 degrees") {
      right_arm_end_fi.SetTotalRotation(sf::degrees(225.f));
      REQUIRE(spatial_analysis::end_of_arm_is_grab_ready(
          right_arm, false, anchor_joint, valid_grab_pkg.part_graph));
    }
    SECTION("rotation 270 degrees") {
      right_arm_end_fi.SetTotalRotation(sf::degrees(270.f));
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
    left_arm_end_fi.SetTotalRotation(sf::degrees(90.f));
    right_arm_end_fi.SetTotalRotation(sf::degrees(90.f));
    REQUIRE(spatial_analysis::all_arms_are_grab_ready(
        grab_result, valid_grab_pkg.part_graph));
  }
  SECTION("left arm is not grab ready") {
    left_arm_end_fi.SetTotalRotation(sf::degrees(135.f));
    REQUIRE_FALSE(spatial_analysis::all_arms_are_grab_ready(
        grab_result, valid_grab_pkg.part_graph));
  }
  SECTION("right arm is not grab ready") {
    right_arm_end_fi.SetTotalRotation(sf::degrees(45.f));
    REQUIRE_FALSE(spatial_analysis::all_arms_are_grab_ready(
        grab_result, valid_grab_pkg.part_graph));
  }
}
} // namespace steamrot::tests
