/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the free functions for the spatial analysis of a grab
/// structure
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "grab_analysis.h"
#include "MachinaFormScaffold.h"
#include "PartGraphBuilder.h"
#include "SocketState.h"
#include "Vector2fEqualsMatcher.h"
#include "action_grimoire_machina.h"
#include "descriptors_machina_archetypes.h"
#include "joint_library.h"
#include "machina_archetype_packages.h"
#include "positioning_grimoire_machina.h"
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
  JointInstance ji_one = JointInstance{&parts::JointSquareWithOneSocket};
  positioning::grimoire_machina::maximise_joint_socket_spread(ji_one);
  const sf::Vector2f &ji_one_socket_0_local_position =
      ji_one.sockets.at(0).local_position;
  REQUIRE(ji_one.transform == sf::Transform::Identity);
  REQUIRE_THAT(ji_one_socket_0_local_position,
               Vector2fEqualsMatcher({19.19f, 19.19f}, 0.01f));
  REQUIRE_THAT(ji_one.transform.transformPoint(ji_one_socket_0_local_position),
               Vector2fEqualsMatcher({19.19f, 19.19f}, 0.01f));

  JointInstance ji_two = JointInstance{&parts::JointSquareWithTwoSockets};
  positioning::grimoire_machina::maximise_joint_socket_spread(ji_two);
  const sf::Vector2f &ji_two_socket_0_local_position =
      ji_two.sockets.at(0).local_position;
  const sf::Vector2f &ji_two_socket_1_local_position =
      ji_two.sockets.at(1).local_position;
  REQUIRE_THAT(ji_two_socket_0_local_position,
               Vector2fEqualsMatcher({23.0f, 10.0f}));
  REQUIRE_THAT(ji_two_socket_1_local_position,
               Vector2fEqualsMatcher({10.0f, 23.0f}));

  JointInstance ji_three = JointInstance{&parts::JointSquareWithThreeSockets};
  positioning::grimoire_machina::maximise_joint_socket_spread(ji_three);
  const sf::Vector2f &ji_three_socket_0_local_position =
      ji_three.sockets.at(0).local_position;
  const sf::Vector2f &ji_three_socket_1_local_position =
      ji_three.sockets.at(1).local_position;
  const sf::Vector2f &ji_three_socket_2_local_position =
      ji_three.sockets.at(2).local_position;
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
          ji_one.transform.transformPoint(ji_one_socket_0_local_position),
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
          ji_one.transform.transformPoint(ji_one_socket_0_local_position),
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
          ji_one.transform.transformPoint(ji_one_socket_0_local_position),
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
          ji_two.transform.transformPoint(ji_two_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(
          ji_two.transform.transformPoint(ji_two_socket_1_local_position),
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
          ji_two.transform.transformPoint(ji_two_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(
          ji_two.transform.transformPoint(ji_two_socket_1_local_position),
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
          ji_two.transform.transformPoint(ji_two_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(
          ji_two.transform.transformPoint(ji_two_socket_1_local_position),
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
      REQUIRE_THAT(
          ji_three.transform.transformPoint(ji_three_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(
          ji_three.transform.transformPoint(ji_three_socket_1_local_position),
          Vector2fEqualsMatcher(expected_socket_1_position, 0.01f));
      REQUIRE_THAT(
          ji_three.transform.transformPoint(ji_three_socket_2_local_position),
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
      REQUIRE_THAT(
          ji_three.transform.transformPoint(ji_three_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(
          ji_three.transform.transformPoint(ji_three_socket_1_local_position),
          Vector2fEqualsMatcher(expected_socket_1_position, 0.01f));
      REQUIRE_THAT(
          ji_three.transform.transformPoint(ji_three_socket_2_local_position),
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
      REQUIRE_THAT(
          ji_three.transform.transformPoint(ji_three_socket_0_local_position),
          Vector2fEqualsMatcher(expected_socket_0_position, 0.01f));
      REQUIRE_THAT(
          ji_three.transform.transformPoint(ji_three_socket_1_local_position),
          Vector2fEqualsMatcher(expected_socket_1_position, 0.01f));
      REQUIRE_THAT(
          ji_three.transform.transformPoint(ji_three_socket_2_local_position),
          Vector2fEqualsMatcher(expected_socket_2_position, 0.01f));
    }
  }
}

TEST_CASE("get_end_of_arm tests") {
  // arrange
  SubGraph arm1{1, 2, 3, 4};
  SubGraph arm2{5};
  SubGraph arm3{};

  // act and assert
  REQUIRE(spatial_analysis::get_end_of_arm(arm1) == 4);
  REQUIRE(spatial_analysis::get_end_of_arm(arm2) == 5);
  REQUIRE(spatial_analysis::get_end_of_arm(arm3) == 0);
}

TEST_CASE("assign_left_and_right_arm_sockets tests") {

  SECTION("assign_left_and_right_arm_sockets with no connected sockets") {
    JointInstance anchor_joint{nullptr};
    anchor_joint.sockets.emplace(0, SocketState{});
    anchor_joint.sockets.emplace(1, SocketState{});
    anchor_joint.sockets.emplace(2, SocketState{});
    anchor_joint.sockets.emplace(3, SocketState{});
    // set all sockets to available
    for (auto &[socket_id, socket_data] : anchor_joint.sockets) {
      socket_data.connection_state = SocketConnectionState::Available;
    }
    std::vector<uint32_t> left_arm_sockets;
    std::vector<uint32_t> right_arm_sockets;
    // act
    spatial_analysis::assign_left_and_right_arm_sockets(
        anchor_joint, left_arm_sockets, right_arm_sockets);
    // assert
    REQUIRE(left_arm_sockets.empty());
    REQUIRE(right_arm_sockets.empty());
  }

  SECTION("assign_left_and_right_arm_sockets with even number of connected "
          "sockets") {
    JointInstance anchor_joint{nullptr};
    anchor_joint.sockets.emplace(0, SocketState{});
    anchor_joint.sockets.emplace(1, SocketState{});
    anchor_joint.sockets.emplace(2, SocketState{});
    anchor_joint.sockets.emplace(3, SocketState{});

    // set sockets 0 and 2 to connected and sockets 1 and 3 to available
    anchor_joint.sockets.at(0).connection_state =
        SocketConnectionState::Connected;
    anchor_joint.sockets.at(1).connection_state =
        SocketConnectionState::Available;
    anchor_joint.sockets.at(2).connection_state =
        SocketConnectionState::Connected;
    anchor_joint.sockets.at(3).connection_state =
        SocketConnectionState::Available;

    std::vector<uint32_t> left_arm_sockets;
    std::vector<uint32_t> right_arm_sockets;

    // act
    spatial_analysis::assign_left_and_right_arm_sockets(
        anchor_joint, left_arm_sockets, right_arm_sockets);

    // assert
    REQUIRE(left_arm_sockets.size() == 1);
    REQUIRE(left_arm_sockets.at(0) == 0);
    REQUIRE(right_arm_sockets.size() == 1);
    REQUIRE(right_arm_sockets.at(0) == 2);
  }

  SECTION("assign_left_and_right_arm_sockets with odd number of connected "
          "sockets") {
    JointInstance anchor_joint{nullptr};
    anchor_joint.sockets.emplace(0, SocketState{});
    anchor_joint.sockets.emplace(1, SocketState{});
    anchor_joint.sockets.emplace(2, SocketState{});
    // set sockets 0 and 1 to connected and socket 2 to available
    anchor_joint.sockets.at(0).connection_state =
        SocketConnectionState::Connected;
    anchor_joint.sockets.at(1).connection_state =
        SocketConnectionState::Connected;
    anchor_joint.sockets.at(2).connection_state =
        SocketConnectionState::Connected;

    std::vector<uint32_t> left_arm_sockets;
    std::vector<uint32_t> right_arm_sockets;
    // act
    spatial_analysis::assign_left_and_right_arm_sockets(
        anchor_joint, left_arm_sockets, right_arm_sockets);
    // assert
    REQUIRE(left_arm_sockets.size() == 1);
    REQUIRE(left_arm_sockets.at(0) == 0);
    REQUIRE(right_arm_sockets.size() == 2);
    REQUIRE(right_arm_sockets.at(0) == 1);
    REQUIRE(right_arm_sockets.at(1) == 2);
  }
}

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
  REQUIRE_THAT(
      anchor_joint.transform.transformPoint(anchor_joint.joint->socket_pivot),
      Vector2fEqualsMatcher(expected_anchor_joint_socket_pivot_position,
                            0.01f));
  sf::Vector2f expected_anchor_joint_socket_0_position{9.19f, 9.19f};
  REQUIRE_THAT(
      anchor_joint.transform.transformPoint(
          anchor_joint.sockets.at(0).local_position),
      Vector2fEqualsMatcher(expected_anchor_joint_socket_0_position, 0.01f));
  sf::Vector2f expected_anchor_joint_socket_1_position{-9.19f, 9.19f};
  REQUIRE_THAT(
      anchor_joint.transform.transformPoint(
          anchor_joint.sockets.at(1).local_position),
      Vector2fEqualsMatcher(expected_anchor_joint_socket_1_position, 0.01f));

  // LEFT ARM //
  const SubGraph &arm_one = grab_result.arms[0];

  /// LEFT ARM: PART ONE ///
  // first part of the arm should be a fragment instance and should be connected
  // to the anchor joint. This should be connected by socket 0 of the anchor
  // joint and socket 1 of the fragment instance
  const auto &arm_one_part_one = graph.at(arm_one[0]);
  REQUIRE(std::holds_alternative<FragmentInstance>(arm_one_part_one));
  const FragmentInstance &arm_one_part_one_fi =
      std::get<FragmentInstance>(arm_one_part_one);
  auto connection_one = action::grimoire_machina::check_for_connected_sockets(
      anchor_joint, arm_one_part_one_fi);
  REQUIRE(connection_one.has_value());
  REQUIRE(connection_one->joint_socket_id == 0);
  REQUIRE(connection_one->fragment_socket_id == 1);
  // arm_one_part_one_fi socket 1 should be at thte same position as
  // anchor_joint socket 0
  sf::Vector2f expected_arm_one_part_one_socket_1_position{9.19f, 9.19f};
  REQUIRE_THAT(arm_one_part_one_fi.transform.transformPoint(
                   arm_one_part_one_fi.sockets.at(1).local_position),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_one_socket_1_position, 0.01f));
  // arm_one_part_one_fi socket 0 should be of length 50 from socket 1 but
  // aligned with the anchor joint socket 0. a
  sf::Vector2f expected_arm_one_part_one_socket_0_position{44.55f, 44.55f};
  REQUIRE_THAT(arm_one_part_one_fi.transform.transformPoint(
                   arm_one_part_one_fi.sockets.at(0).local_position),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_one_socket_0_position, 0.01f));

  // LEFT ARM:: PART TWO ///
  // the second part of the arm should be a joint instance and should be
  // connected to the first part of the arm. This should be connected by socket
  // 0 of the first part of the arm and socket 1 of the second part of the arm
  const auto &arm_one_part_two = graph.at(arm_one[1]);
  REQUIRE(std::holds_alternative<JointInstance>(arm_one_part_two));
  const JointInstance &arm_one_part_two_ji =
      std::get<JointInstance>(arm_one_part_two);
  auto connection_two = action::grimoire_machina::check_for_connected_sockets(
      arm_one_part_two_ji, arm_one_part_one_fi);
  REQUIRE(connection_two.has_value());
  REQUIRE(connection_two->joint_socket_id == 1);
  REQUIRE(connection_two->fragment_socket_id == 0);
  // arm_one_part_two_ji socket 1 should be at the same position as
  // arm_one_part_one_fi socket 0
  sf::Vector2f expected_arm_one_part_two_socket_1_position{44.55f, 44.55f};
  REQUIRE_THAT(arm_one_part_two_ji.transform.transformPoint(
                   arm_one_part_two_ji.sockets.at(1).local_position),
               Vector2fEqualsMatcher(
                   expected_arm_one_part_two_socket_1_position, 0.01f));

  // RIGHT ARM //
  const SubGraph &arm_two = grab_result.arms[1];

  /// RIGHT ARM: PART ONE ///
  /// first part of the arm should be a fragment instance and should be
  /// connected to the anchor joint. This should be connected by socket 1 of the
  /// anchor joint and socket 0 of the fragment instance
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
  // arm_two_part_one_fi socket 0 should be at thte same position as
  // anchor_joint socket 1
  sf::Vector2f expected_arm_two_part_one_socket_0_position{-9.19f, 9.19f};
  REQUIRE_THAT(arm_two_part_one_fi.transform.transformPoint(
                   arm_two_part_one_fi.sockets.at(0).local_position),
               Vector2fEqualsMatcher(
                   expected_arm_two_part_one_socket_0_position, 0.01f));
  sf::Vector2f expected_arm_two_part_one_socket_1_position{-44.55f, 44.55f};
  REQUIRE_THAT(arm_two_part_one_fi.transform.transformPoint(
                   arm_two_part_one_fi.sockets.at(1).local_position),
               Vector2fEqualsMatcher(
                   expected_arm_two_part_one_socket_1_position, 0.01f));
}
} // namespace steamrot::tests
