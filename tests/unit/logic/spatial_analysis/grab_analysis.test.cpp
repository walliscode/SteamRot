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
#include "Vector2fEqualsMatcher.h"
#include "descriptors_machina_archetypes.h"
#include "machina_archetype_packages.h"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
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

TEST_CASE("align_grab_structure tests") {
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

  SECTION("align_grab_structure sets the anchor's transform to the provided "
          "vector") {
    // arrange
    // pull out the transform of the anchor joint instance, we can use this to
    // check if the function is actually changing it
    sf::Transform &anchor_transform = anchor_instance.transform;
    // add an arbitrary transform to the anchor joint instance to make sure the
    // function is actually changing it
    anchor_transform.translate({100.f, 50.f});
    REQUIRE(anchor_transform.transformPoint({0.f, 0.f}) ==
            sf::Vector2f{100.f, 50.f});

    // act
    std::vector<sf::Vector2f> target_positions{
        {{0.f, 0.f}, {0.f, 100.f}, {65.f, 25.f}, {-50.f, -50.f}}};

    for (const sf::Vector2f &target_position : target_positions) {
      spatial_analysis::align_grab_structure(grab_result, graph,
                                             target_position);
      // assert
      // the transform of the anchor joint should be able to move a 0,0 point to
      // the target position
      REQUIRE_THAT(anchor_transform.transformPoint({0.f, 0.f}),
                   EqualsVector2f(target_position));
    }
  }

  SECTION("align_grab_structure rotates the anchor joint correctly") {
    // The anchor joint should be rotated such that the middle of its available
    // arc aligns with the y axis this particular Joint has arc_min = 0  and
    // arc_max = 270

    // arrange

    // pull out the transform of the anchor joint instance, we can use this to
    // check if the function is actually changing it
    sf::Transform &anchor_transform = anchor_instance.transform;

    // get rotation values from the anchor Joint pointer
    const float arc_min = anchor_instance.joint->socket_config.rotation_arc_min;
    const float arc_max = anchor_instance.joint->socket_config.rotation_arc_max;
    const float arc_mid = (arc_min + arc_max) / 2.f;

    // act
    spatial_analysis::align_grab_structure(grab_result, graph, {0.f, 0.f});

    // assert
    // construct a transform that rotates by the arc_mid value and check that it
    // is equal to the anchor joint's transform
    sf::Transform expected_transform;
    expected_transform.rotate(sf::degrees(90 - arc_mid));

    REQUIRE(anchor_transform == expected_transform);
  }

  SECTION("align_grab_structure translates and rotates the anchor joint "
          "correctly") {
    // The anchor joint should be translated to the provided position and
    // rotated such that the middle of its available arc aligns with the y axis
    // this particular Joint has arc_min = 0  and arc_max = 270
    // arrange
    // pull out the transform of the anchor joint instance, we can use this to
    // check if the function is actually changing it
    sf::Transform &anchor_transform = anchor_instance.transform;
    // get rotation values from the anchor Joint pointer
    const float arc_min = anchor_instance.joint->socket_config.rotation_arc_min;
    const float arc_max = anchor_instance.joint->socket_config.rotation_arc_max;
    const float arc_mid = (arc_min + arc_max) / 2.f;
    // act
    std::vector<sf::Vector2f> target_positions{
        {{0.f, 0.f}, {0.f, 100.f}, {65.f, 25.f}, {-50.f, -50.f}}};
    for (const sf::Vector2f &target_position : target_positions) {
      spatial_analysis::align_grab_structure(grab_result, graph,
                                             target_position);
      // assert
      // construct a transform that translates to the target position and then
      // rotates by the arc_mid value and check that it is equal to the anchor
      // joint's transform
      sf::Transform expected_transform;
      expected_transform.translate(target_position);
      expected_transform.rotate(sf::degrees(90 - arc_mid));
      REQUIRE(anchor_transform == expected_transform);
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

TEST_CASE("assign_open_state_transforms tests") {
  // arrange
  PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  MachinaArchetypeResult ma_result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);
  REQUIRE(std::holds_alternative<GrabResult>(ma_result.result_sub_graphs));

  // grab result contains the anchor joint and the arms of the grab structure
  GrabResult grab_result = std::get<GrabResult>(ma_result.result_sub_graphs);
  const uint32_t &anchor_id = grab_result.anchor;
  PartGraph &graph = valid_grab_pkg.part_graph;

  SECTION("assign_open_state_transforms is empty if not run through function") {
    REQUIRE(grab_result.static_transforms.empty());
  }

  SECTION(
      "assign_open_state_transforms assigns transforms for the anchor joint") {
    // arrange
    REQUIRE(std::holds_alternative<JointInstance>(graph.at(anchor_id)));
    JointInstance &anchor_joint =
        spatial_analysis::get_anchor_joint(grab_result, graph);
    const SocketConfig &config = anchor_joint.joint->socket_config;
    REQUIRE(config.socket_count == 2);
    anchor_joint.sockets.at(0).local_position = {0.f, 0.f};
    anchor_joint.sockets.at(1).local_position = {0.f, 0.f};

    // act
    spatial_analysis::assign_open_state_transforms(grab_result, graph);

    // assert
    // the anchor joint should have its sockets at its maximum angles, manually
    // calculate the expected positions and then add to the origin
    sf::Vector2f expected_position_0 =
        anchor_joint.joint->origin +
        sf::Vector2f{
            config.radius *
                std::cos(sf::degrees(config.rotation_arc_min).asRadians()),
            config.radius *
                std::sin(sf::degrees(config.rotation_arc_min).asRadians())};

    sf::Vector2f expected_position_1 =
        anchor_joint.joint->origin +
        sf::Vector2f{
            config.radius *
                std::cos(sf::degrees(config.rotation_arc_max).asRadians()),
            config.radius *
                std::sin(sf::degrees(config.rotation_arc_max).asRadians())};

    REQUIRE_THAT(anchor_joint.sockets.at(0).local_position,
                 EqualsVector2f(expected_position_0, 0.001f));
    REQUIRE_THAT(anchor_joint.sockets.at(1).local_position,
                 EqualsVector2f(expected_position_1, 0.001f));
  }
}
} // namespace steamrot::tests
