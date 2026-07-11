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
#include "TransformEqualsMatcher.h"
#include "Vector2fEqualsMatcher.h"
#include "descriptors_machina_archetypes.h"
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
  // arrange
  // set up valid grab package and result
  PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  MachinaArchetypeResult ma_result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);

  // pull ou the GrabResult from the MachinaArchetypeResult variant
  REQUIRE(std::holds_alternative<GrabResult>(ma_result.result_sub_graphs));
  GrabResult grab_result = std::get<GrabResult>(ma_result.result_sub_graphs);
  PartGraph &graph = valid_grab_pkg.part_graph;

  // get the instance of the anchor joint
  const uint32_t anchor_id = grab_result.anchor;
  REQUIRE(valid_grab_pkg.id_to_part_graph_id.at("j3") == anchor_id);
  REQUIRE(std::holds_alternative<JointInstance>(graph.at(anchor_id)));
  JointInstance &anchor_instance = std::get<JointInstance>(graph.at(anchor_id));

  // act
  spatial_analysis::align_anchor_joint_to_anchor_point(grab_result, graph,
                                                       {0.f, 0.f});
  // assert
  sf::Transform expected_transform{sf::Transform::Identity};
  // origin of the anchor joint is at (10, 10) in local space, so to align it to
  // (0, 0) in world space, we need to translate by (-10, -10)
  expected_transform.translate({-10.f, -10.f});

  REQUIRE_THAT(anchor_instance.transform,
               TransformEqualsMatcher(expected_transform));
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
  // arrange
  PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  MachinaArchetypeResult ma_result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);
  REQUIRE(std::holds_alternative<GrabResult>(ma_result.result_sub_graphs));
  GrabResult grab_result = std::get<GrabResult>(ma_result.result_sub_graphs);
  PartGraph &graph = valid_grab_pkg.part_graph;

  // assert
  JointInstance &anchor_joint =
      std::get<JointInstance>(graph.at(grab_result.anchor));

  SECTION("align_grab_result_to_open_state sets the anchor joint to the open "
          "state") {
    // act
    spatial_analysis::align_grab_result_to_open_state(grab_result, graph,
                                                      {0.f, 0.f});
    // assert
  }
}
} // namespace steamrot::tests
