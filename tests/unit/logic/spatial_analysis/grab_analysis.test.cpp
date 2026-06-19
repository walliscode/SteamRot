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
#include <variant>

namespace steamrot::tests {
using namespace steamrot::logic;

TEST_CASE("valid_grab_pkg passes grab structural tests") {
  steamrot::tests::PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  MachinaArchetypeResult result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);

  REQUIRE(result);
}

TEST_CASE("align_grab_structure tests") {
  // arrange
  // set up valid grab package and result
  steamrot::tests::PartGraphPackage valid_grab_pkg = create_valid_grab_pkg();
  steamrot::MachinaArchetypeResult ma_result =
      descriptors::MA::Grab()(valid_grab_pkg.part_graph,
                              valid_grab_pkg.id_to_part_graph_id.at("j3"), 0);

  // pull ou the GrabResult from the MachinaArchetypeResult variant
  REQUIRE(std::holds_alternative<steamrot::GrabResult>(
      ma_result.result_sub_graphs));
  steamrot::GrabResult grab_result =
      std::get<steamrot::GrabResult>(ma_result.result_sub_graphs);
  steamrot::PartGraph &graph = valid_grab_pkg.part_graph;

  // get the instance of the anchor joint
  const uint32_t anchor_id = grab_result.anchor;
  REQUIRE(valid_grab_pkg.id_to_part_graph_id.at("j3") == anchor_id);
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(graph.at(anchor_id)));
  steamrot::JointInstance &anchor_instance =
      std::get<steamrot::JointInstance>(graph.at(anchor_id));

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
      steamrot::logic::spatial_analysis::align_grab_structure(
          grab_result, graph, target_position);
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
    steamrot::logic::spatial_analysis::align_grab_structure(grab_result, graph,
                                                            {0.f, 0.f});

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
      steamrot::logic::spatial_analysis::align_grab_structure(
          grab_result, graph, target_position);
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
  REQUIRE(steamrot::logic::spatial_analysis::get_end_of_arm(arm1) == 4);
  REQUIRE(steamrot::logic::spatial_analysis::get_end_of_arm(arm2) == 5);
  REQUIRE(steamrot::logic::spatial_analysis::get_end_of_arm(arm3) == 0);
}

TEST_CASE("assign_left_and_right_arm_sockets tests") {

  SECTION("assign_left_and_right_arm_sockets with no connected sockets") {
    JointInstance anchor_joint{nullptr};
    anchor_joint.sockets.emplace(0, SocketData{{0.f, 0.f}});
    anchor_joint.sockets.emplace(1, SocketData{{0.f, 0.f}});
    anchor_joint.sockets.emplace(2, SocketData{{0.f, 0.f}});
    anchor_joint.sockets.emplace(3, SocketData{{0.f, 0.f}});
    // set all sockets to available
    for (auto &[socket_id, socket_data] : anchor_joint.sockets) {
      socket_data.state = SocketState::Available;
    }
    std::vector<uint32_t> left_arm_sockets;
    std::vector<uint32_t> right_arm_sockets;
    // act
    steamrot::logic::spatial_analysis::assign_left_and_right_arm_sockets(
        anchor_joint, left_arm_sockets, right_arm_sockets);
    // assert
    REQUIRE(left_arm_sockets.empty());
    REQUIRE(right_arm_sockets.empty());
  }

  SECTION("assign_left_and_right_arm_sockets with even number of connected "
          "sockets") {
    JointInstance anchor_joint{nullptr};
    anchor_joint.sockets.emplace(0, SocketData{{0.f, 0.f}});
    anchor_joint.sockets.emplace(1, SocketData{{0.f, 0.f}});
    anchor_joint.sockets.emplace(2, SocketData{{0.f, 0.f}});
    anchor_joint.sockets.emplace(3, SocketData{{0.f, 0.f}});

    // set sockets 0 and 2 to connected and sockets 1 and 3 to available
    anchor_joint.sockets.at(0).state = SocketState::Connected;
    anchor_joint.sockets.at(1).state = SocketState::Available;
    anchor_joint.sockets.at(2).state = SocketState::Connected;
    anchor_joint.sockets.at(3).state = SocketState::Available;

    std::vector<uint32_t> left_arm_sockets;
    std::vector<uint32_t> right_arm_sockets;

    // act
    steamrot::logic::spatial_analysis::assign_left_and_right_arm_sockets(
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
    anchor_joint.sockets.emplace(0, SocketData{{0.f, 0.f}});
    anchor_joint.sockets.emplace(1, SocketData{{0.f, 0.f}});
    anchor_joint.sockets.emplace(2, SocketData{{0.f, 0.f}});
    // set sockets 0 and 1 to connected and socket 2 to available
    anchor_joint.sockets.at(0).state = SocketState::Connected;
    anchor_joint.sockets.at(1).state = SocketState::Connected;
    anchor_joint.sockets.at(2).state = SocketState::Connected;

    std::vector<uint32_t> left_arm_sockets;
    std::vector<uint32_t> right_arm_sockets;
    // act
    steamrot::logic::spatial_analysis::assign_left_and_right_arm_sockets(
        anchor_joint, left_arm_sockets, right_arm_sockets);
    // assert
    REQUIRE(left_arm_sockets.size() == 1);
    REQUIRE(left_arm_sockets.at(0) == 0);
    REQUIRE(right_arm_sockets.size() == 2);
    REQUIRE(right_arm_sockets.at(0) == 1);
    REQUIRE(right_arm_sockets.at(1) == 2);
  }
}
} // namespace steamrot::tests
