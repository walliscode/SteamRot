/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning grimoire machina free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "Fragment.h"
#include "JointInstance.h"
#include "MachinaFormScaffold.h"
#include "PartGraphBuilder.h"
#include "Vector2fEqualsMatcher.h"
#include "ViewDirection.h"
#include "fragment_library.h"
#include "joint_library.h"
#include <SFML/Graphics.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace steamrot::tests {

using namespace steamrot::logic::positioning::grimoire_machina;

TEST_CASE("position_first_part_of_machina_form tests",
          "[positioning_grimoire_machina]") {
  // Arrange
  PartGraph parts;

  SECTION("Does not throw when parts is empty") {
    // Act & Assert
    REQUIRE_NOTHROW(position_first_part_of_machina_form_scaffold(parts));
  }

  SECTION("Does not throw when Fragment has no sockets") {
    // Arrange
    Fragment fragment{};
    FragmentInstance fragment_instance{0, fragment};
    parts.emplace(fragment_instance.GetId(), fragment_instance);

    REQUIRE(parts.size() == 1); // sanity check
    // Act & Assert
    REQUIRE_NOTHROW(position_first_part_of_machina_form_scaffold(parts));
  }

  SECTION("Positions centre of first Fragmentinstance's FRONT view at 0,0") {
    // Arrange

    FragmentInstance fragment_instance{0,
                                       parts::FragmentRectangleWithOneSocket};
    parts.emplace(fragment_instance.GetId(), fragment_instance);
    REQUIRE(parts.size() == 1); // sanity check
    // pull out reference to the Fragmentinstance we just added so we can check
    // its transform after
    FragmentInstance &instance = std::get<FragmentInstance>(parts.at(0));

    // Act
    position_first_part_of_machina_form_scaffold(parts);

    // Assert
    REQUIRE_THAT(fragment_instance.getPosition(),
                 EqualsVector2f(sf::Vector2f{0.f, 0.f}));
  }

  SECTION("Positions origin of first JointInstance at 0,0") {
    // Arrange
    JointInstance joint_instance{0, parts::JointSquareWithOneSocket};
    parts.emplace(joint_instance.GetId(), joint_instance);
    REQUIRE(parts.size() == 1); // sanity check
    //
    // pull out reference to the JointInstance we just added so we can check
    // its transform after
    JointInstance &instance = std::get<JointInstance>(parts.at(0));
    // Act

    position_first_part_of_machina_form_scaffold(parts);

    // we expect the joint origin to be translated to {0,0}
    sf::Vector2f expected_position{0.f, 0.f};

    REQUIRE_THAT(joint_instance.GetSocketPivotWorldPosition(),
                 EqualsVector2f(expected_position));
  }
}

TEST_CASE("rotate_vector_to_target_vector tests") {

  SECTION("Does not throw with zero vectors") {
    sf::Vector2f source_vector{0.f, 0.f};
    sf::Vector2f target_vector{0.f, 0.f};

    REQUIRE_NOTHROW(
        rotation_of_vector_to_target_vector(source_vector, target_vector));
  }
  SECTION("Rotates vector to target vector") {

    // this is SFML space so y axis is inverted, so up is negative y and down is
    // positive y
    std::vector<sf::Vector2f> source_vectors{
        {1.f, 0.f},  // pointing right
        {0.f, -1.f}, // pointing up
        {-1.f, 0.f}, // pointing left
        {0.f, 1.f},  // pointing down
        {1.f, 1.f},  // pointing down-right
        {-1.f, -1.f} // pointing up-left
    };
    sf::Vector2f target_vector{0.f, 1.f}; // pointing down

    for (const auto &source_vector : source_vectors) {

      // create a transform that rotates the source vector to the target vector
      sf::Transform transform = sf::Transform::Identity;
      transform.rotate(
          rotation_of_vector_to_target_vector(source_vector, target_vector));

      // The transform should now rotate the source vector to the target vector,
      // normalise to avoid new magnitude issues
      sf::Vector2f transformed_source =
          transform.transformPoint(source_vector).normalized();
      ;

      REQUIRE_THAT(transformed_source, steamrot::tests::EqualsVector2f(
                                           target_vector.normalized(), 0.001f));
    }
  }
}

// TEST_CASE("check_if_allowed_joint_socket_configuration tests",
//           "[unit][positioning_grimoire_machina]") {
//
//   SECTION("Returns false if any socket is out of bounds by radius") {
//
//     steamrot::Joint joint;
//     joint.socket_pivot = {0.f, 0.f};
//     joint.socket_config.socket_count = 3;
//     joint.socket_config.radius = 10.f;
//     joint.socket_config.rotation_arc_min = 0.f;
//     joint.socket_config.rotation_arc_max = 270.f;
//     steamrot::JointInstance instance{&joint};
//     steamrot::logic::positioning::grimoire_machina::
//         initialize_joint_socket_positions(instance);
//
//     // Manually set one socket to be out of bounds
//     instance.sockets.at(1).local_position = {13.f, 0.f};
//     REQUIRE_FALSE(steamrot::logic::positioning::grimoire_machina::
//                       check_if_allowed_joint_socket_configuration(instance));
//   }
//
//   SECTION("Returns false if any socket is out of bounds by rotation arc") {
//
//     struct TestCase {
//       std::string name;
//       float rotation_arc_min;
//       float rotation_arc_max;
//       sf::Vector2f out_of_bounds_socket_position;
//     };
//     steamrot::Joint joint;
//     joint.socket_pivot = {0.f, 0.f};
//     joint.socket_config.socket_count = 3;
//     joint.socket_config.radius = 10.f;
//     steamrot::JointInstance instance{&joint};
//     steamrot::logic::positioning::grimoire_machina::
//         initialize_joint_socket_positions(instance);
//
//     // Test cases for out-of-bounds sockets
//     std::vector<TestCase> test_cases = {
//         {"Socket below min arc", 45.f, 270.f, {10.f, 0.f}},
//         {"Socket above max arc", 0.f, 180.f, {0.f, -10.f}},
//     };
//
//     for (const auto &tc : test_cases) {
//       DYNAMIC_SECTION(tc.name) {
//         joint.socket_config.rotation_arc_min = tc.rotation_arc_min;
//         joint.socket_config.rotation_arc_max = tc.rotation_arc_max;
//         // set one socket to be out of bounds
//         instance.sockets.at(1).local_position =
//             tc.out_of_bounds_socket_position;
//         REQUIRE_FALSE(
//             steamrot::logic::positioning::grimoire_machina::
//                 check_if_allowed_joint_socket_configuration(instance));
//       }
//     }
//   }
//
//   SECTION("Returns false if minimum_gap is not maintained between sockets") {
//     // Arrange
//     steamrot::Joint joint;
//     joint.socket_pivot = {0.f, 0.f};
//     joint.socket_config.socket_count = 3;
//     joint.socket_config.radius = 10.f;
//     joint.socket_config.rotation_arc_min = 0.f;
//     joint.socket_config.rotation_arc_max = 180.f;
//     steamrot::JointInstance instance{&joint};
//
//     // set up a test struct to initialize the joint socket positions
//     struct TestStruct {
//       std::string name;
//       float minimum_gap;
//       sf::Vector2f socket_0_position;
//       sf::Vector2f socket_1_position;
//       sf::Vector2f socket_2_position;
//     };
//
//     std::vector<TestStruct> test_cases = {
//         {"Sockets too close together_one",
//          15.f,
//          {10.f, 0.f},
//          {9.510565f, 3.090170f},
//          {8.660254f, 5.f}},
//         {"Sockets too close together_two",
//          10.f,
//          {10.f, 0.f},
//          {9.238795f, 3.826834f},
//          {8.660254f, 5.f}},
//     };
//
//     for (const auto &tc : test_cases) {
//       DYNAMIC_SECTION(tc.name) {
//         joint.socket_config.minimum_gap = tc.minimum_gap;
//         instance.sockets.at(0).local_position = tc.socket_0_position;
//         instance.sockets.at(1).local_position = tc.socket_1_position;
//         instance.sockets.at(2).local_position = tc.socket_2_position;
//         REQUIRE_FALSE(
//             steamrot::logic::positioning::grimoire_machina::
//                 check_if_allowed_joint_socket_configuration(instance));
//       }
//     }
//   }
//
//   SECTION("Returns false if socket order is not maintained (clockwise)") {
//     // Arrange
//     steamrot::Joint joint;
//     joint.socket_pivot = {0.f, 0.f};
//     joint.socket_config.socket_count = 3;
//     joint.socket_config.radius = 10.f;
//     joint.socket_config.rotation_arc_min = 0.f;
//     joint.socket_config.rotation_arc_max = 270.f;
//     joint.socket_config.minimum_gap = 5.f;
//     steamrot::JointInstance instance{&joint};
//     // Manually set sockets to be out of order (clockwise)
//     instance.sockets.at(0).local_position = {10.f, 0.f}; // 0°
//     // set socket 1 to be at 270° (out of order)
//     instance.sockets.at(1).local_position = {0.f, -10.f}; // 270°
//     // set socket 2 to be at 135° (in order)
//     instance.sockets.at(2).local_position = {-7.071f, 7.071f}; // 135°
//     // Act & Assert
//     REQUIRE_FALSE(steamrot::logic::positioning::grimoire_machina::
//                       check_if_allowed_joint_socket_configuration(instance));
//   }
// }

TEST_CASE("position_from_node tests", "[unit][positioning_grimoire_machina]") {

  // Arrange
  std::unordered_set<uint32_t> visited;
  std::unordered_set<uint32_t> in_stack;
  SECTION("returns std::monostate when part graph is empty") {
    PartGraph part_graph;
    auto position_result = position_from_node(part_graph, 0, visited, in_stack);
  }

  SECTION("Returns unexpected when part_id does not exist in part graph") {
    PartGraph part_graph;
    FragmentInstance fragment_instance{1,
                                       parts::FragmentRectangleWithOneSocket};
    part_graph.emplace(fragment_instance.GetId(), fragment_instance);

    auto position_result = position_from_node(part_graph, 0, visited, in_stack);
    REQUIRE_FALSE(position_result.has_value());
    REQUIRE(position_result.error().mode == FailMode::BadValue);
    REQUIRE(position_result.error().message ==
            "part_id 0 does not exist in part graph");
  }
  SECTION("Positions a simple part graph with one fragment and one joint") {

    // ARRANGE //
    std::unordered_set<uint32_t> visited;
    std::unordered_set<uint32_t> in_stack;

    PartGraphBuilder builder;
    PartGraphPackage part_graph_package =
        builder.AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f1")
            .Connect("f1", 0, "j0", 0)
            .Build();

    PartGraph &part_graph = part_graph_package.part_graph;
    // get references to the fragment and joint instances
    auto &joint_instance_result =
        part_graph.at(part_graph_package.id_to_part_graph_id.at("j0"));
    REQUIRE(std::holds_alternative<JointInstance>(joint_instance_result));
    JointInstance &joint_instance =
        std::get<JointInstance>(joint_instance_result); // get joint instance
    REQUIRE(joint_instance.GetId() == 0);
    joint_instance.PositionSockets(
        JointSocketPositioningStrategy::MaximizeDistance);
    //
    auto &fragment_instance_result =
        part_graph.at(part_graph_package.id_to_part_graph_id.at("f1"));
    REQUIRE(std::holds_alternative<FragmentInstance>(fragment_instance_result));
    FragmentInstance &fragment_instance =
        std::get<FragmentInstance>(fragment_instance_result); // get fragment
    REQUIRE(fragment_instance.GetId() == 1);
    REQUIRE(joint_instance
                .CheckForFirstConnectionWithOtherInstance(fragment_instance)
                .has_value()); // ensure connection exists

    // check positions before //
    const sf::Vector2f expected_ji_socket_0_world_before{9.19f, 9.19f};
    REQUIRE_THAT(joint_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_ji_socket_0_world_before, 0.01f));

    const sf::Vector2f expected_ji_socket_pivot_world_before{0.f, 0.f};
    REQUIRE_THAT(joint_instance.GetSocketPivotWorldPosition(),
                 EqualsVector2f(expected_ji_socket_pivot_world_before, 0.01f));

    const sf::Vector2f expected_fi_socket_0_world_before{0.f, 5.f};
    REQUIRE_THAT(fragment_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_fi_socket_0_world_before, 0.01f));

    // ACT //

    auto result = position_from_node(part_graph, 0, visited, in_stack);
    if (!result.has_value()) {
      FAIL("position_from_node failed with error: " + result.error().message);
    }

    // ASSERT //
    // joint and joint sockets should not have moved but the fragment should
    // have been positioned onto the joint socket via the connection
    REQUIRE_THAT(joint_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_ji_socket_0_world_before, 0.01f));
    REQUIRE_THAT(joint_instance.GetSocketPivotWorldPosition(),
                 EqualsVector2f(expected_ji_socket_pivot_world_before, 0.01f));

    const sf::Vector2f expected_fi_socket_0_world_after{9.19f, 9.19f};
    REQUIRE_THAT(fragment_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_fi_socket_0_world_after, 0.01f));

    // rotates 45 degrees to align the fragment socket with the joint socket
    REQUIRE(fragment_instance.getRotation().asDegrees() == 45.f);
  }
  SECTION("Positions a chain graph: j0 - f1 - j2") {

    // Arrange
    PartGraphBuilder builder;
    PartGraphPackage pkg =
        builder.AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithTwoSockets, "f1")
            .AddJointInstance(parts::JointSquareWithOneSocket, "j2")
            .Connect("f1", 0, "j0", 0)
            .Connect("f1", 1, "j2", 0)
            .Build();

    PartGraph &g = pkg.part_graph;

    auto &j0v = g.at(pkg.id_to_part_graph_id.at("j0"));
    auto &f1v = g.at(pkg.id_to_part_graph_id.at("f1"));
    auto &j2v = g.at(pkg.id_to_part_graph_id.at("j2"));

    auto &j0 = std::get<JointInstance>(j0v);
    auto &f1 = std::get<FragmentInstance>(f1v);
    auto &j2 = std::get<JointInstance>(j2v);
    REQUIRE(j0.GetId() == 0);
    REQUIRE(f1.GetId() == 1);
    REQUIRE(j2.GetId() == 2);

    j0.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
    j2.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);

    // Act
    auto result = position_from_node(g, j0.GetId(), visited, in_stack);
    if (!result.has_value()) {
      FAIL("position_from_node failed with error: " + result.error().message);
    }

    // j0 socket positions
    REQUIRE_THAT(j0.GetSocketWorldPosition(0),
                 EqualsVector2f(sf::Vector2f{9.19f, 9.19f}, 0.01f));
    REQUIRE_THAT(j0.GetSocketPivotWorldPosition(),
                 EqualsVector2f(sf::Vector2f{0.f, 0.f}, 0.01f));

    // f1 socket 0 should align to j0 socket 0
    REQUIRE_THAT(f1.GetSocketWorldPosition(0),
                 EqualsVector2f(sf::Vector2f{9.19f, 9.19f}, 0.01f));
    REQUIRE_THAT(f1.GetSocketWorldPosition(1),
                 EqualsVector2f(sf::Vector2f{44.55f, 44.55f}, 0.01f));
    REQUIRE(f1.getRotation().asDegrees() == 45.f);

    // j2 socket positions
    REQUIRE_THAT(j2.GetSocketWorldPosition(0),
                 EqualsVector2f(sf::Vector2f{44.55f, 44.55f}, 0.01f));
    REQUIRE_THAT(j2.GetSocketPivotWorldPosition(),
                 EqualsVector2f(sf::Vector2f{53.74f, 53.74f}, 0.01f));
    REQUIRE(j2.getRotation().asDegrees() == 180.f);
  }
  SECTION("Positions a branching graph: j0 connected to f1 and f2") {
    // Arrange
    PartGraphBuilder builder;
    PartGraphPackage pkg =
        builder.AddJointInstance(parts::JointSquareWithTwoSockets, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f1")
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f2")
            .Connect("f1", 0, "j0", 0)
            .Connect("f2", 0, "j0", 1)
            .Build();

    PartGraph &g = pkg.part_graph;

    auto &j0 = std::get<JointInstance>(g.at(pkg.id_to_part_graph_id.at("j0")));
    j0.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
    auto &f1 =
        std::get<FragmentInstance>(g.at(pkg.id_to_part_graph_id.at("f1")));
    auto &f2 =
        std::get<FragmentInstance>(g.at(pkg.id_to_part_graph_id.at("f2")));

    // Act
    auto result = position_from_node(g, j0.GetId(), visited, in_stack);
    if (!result.has_value()) {
      FAIL("position_from_node failed with error: " + result.error().message);
    }

    // Assert
    // j0 positions
    REQUIRE_THAT(j0.GetSocketWorldPosition(0),
                 EqualsVector2f(sf::Vector2f{13.f, 0.f}, 0.01f));
    REQUIRE_THAT(j0.GetSocketWorldPosition(1),
                 EqualsVector2f(sf::Vector2f{0.f, 13.f}, 0.01f));
    REQUIRE_THAT(j0.GetSocketPivotWorldPosition(),
                 EqualsVector2f(sf::Vector2f{0.f, 0.f}, 0.01f));
    // f1 positions
    REQUIRE_THAT(f1.GetSocketWorldPosition(0),
                 EqualsVector2f({13.f, 0.f}, 0.01f));
    REQUIRE(f1.getRotation().asDegrees() == 0.f);

    REQUIRE_THAT(f2.GetSocketWorldPosition(0),
                 EqualsVector2f({0.f, 13.f}, 0.01f));
    REQUIRE(f2.getRotation().asDegrees() == 90.f);
  }
}
TEST_CASE("position_part_graph_from_first_added tests",
          "[unit][positioning_grimoire_machina]") {

  SECTION("Returns std::monostate when part graph is empty") {
    steamrot::PartGraph part_graph;
    auto result = position_part_graph_from_first_added(part_graph);
    REQUIRE(result.has_value());
  }

  SECTION("Positions a simple part graph with one fragment and one joint") {

    // ARRANGE //
    PartGraphBuilder builder;
    PartGraphPackage part_graph_package =
        builder.AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f1")
            .Connect("f1", 0, "j0", 0)
            .Build();

    PartGraph &part_graph = part_graph_package.part_graph;
    // get references to the fragment and joint instances
    auto &joint_instance_result =
        part_graph.at(part_graph_package.id_to_part_graph_id.at("j0"));
    REQUIRE(std::holds_alternative<JointInstance>(joint_instance_result));
    JointInstance &joint_instance =
        std::get<JointInstance>(joint_instance_result); // get joint instance
    joint_instance.PositionSockets(
        JointSocketPositioningStrategy::MaximizeDistance);
    //
    auto &fragment_instance_result =
        part_graph.at(part_graph_package.id_to_part_graph_id.at("f1"));
    REQUIRE(std::holds_alternative<FragmentInstance>(fragment_instance_result));
    FragmentInstance &fragment_instance =
        std::get<FragmentInstance>(fragment_instance_result); // get fragment
    REQUIRE(joint_instance
                .CheckForFirstConnectionWithOtherInstance(fragment_instance)
                .has_value()); // ensure connection exists

    // check positions before //
    const sf::Vector2f expected_ji_socket_0_world_before{9.19f, 9.19f};
    REQUIRE_THAT(joint_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_ji_socket_0_world_before, 0.01f));
    const sf::Vector2f expected_ji_socket_pivot_world_before{0.f, 0.f};
    REQUIRE_THAT(joint_instance.GetSocketPivotWorldPosition(),
                 EqualsVector2f(expected_ji_socket_pivot_world_before, 0.01f));

    const sf::Vector2f expected_fi_socket_0_world_before{0.f, 5.f};
    REQUIRE_THAT(fragment_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_fi_socket_0_world_before, 0.01f));

    // ACT //
    auto result = position_part_graph_from_first_added(part_graph);
    if (!result.has_value()) {
      FAIL("position_part_graph_from_first_added failed with error: " +
           result.error().message);
    }

    // ASSERT //
    // joint and joint sockets should not have moved but the fragment should
    // have been positioned onto the joint socket via the connection
    REQUIRE_THAT(joint_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_ji_socket_0_world_before, 0.01f));
    REQUIRE_THAT(joint_instance.GetSocketPivotWorldPosition(),
                 EqualsVector2f(expected_ji_socket_pivot_world_before, 0.01f));

    const sf::Vector2f expected_fi_socket_0_world_after{9.19f, 9.19f};
    REQUIRE_THAT(fragment_instance.GetSocketWorldPosition(0),
                 EqualsVector2f(expected_fi_socket_0_world_after, 0.01f));

    // rotates 45 degrees to align the fragment socket with the joint socket
    REQUIRE(fragment_instance.getRotation().asDegrees() == 45.f);
  }
  SECTION("Positions a chain graph: j0 - f1 - j2") {
    PartGraphBuilder builder;
    PartGraphPackage pkg =
        builder.AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithTwoSockets, "f1")
            .AddJointInstance(parts::JointSquareWithOneSocket, "j2")
            .Connect("f1", 0, "j0", 0)
            .Connect("f1", 1, "j2", 0)
            .Build();

    PartGraph &g = pkg.part_graph;

    auto &j0v = g.at(pkg.id_to_part_graph_id.at("j0"));
    auto &f1v = g.at(pkg.id_to_part_graph_id.at("f1"));
    auto &j2v = g.at(pkg.id_to_part_graph_id.at("j2"));

    auto &j0 = std::get<JointInstance>(j0v);
    auto &f1 = std::get<FragmentInstance>(f1v);
    auto &j2 = std::get<JointInstance>(j2v);

    j0.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
    j2.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);

    auto result = position_part_graph_from_first_added(g);

    // f1 socket 0 should align to j0 socket 0
    REQUIRE_THAT(f1.GetSocketWorldPosition(0),
                 EqualsVector2f(j0.GetSocketWorldPosition(0), 0.01f));
  }

  SECTION("Positions a branching graph: j0 connected to f1 and f2") {
    PartGraphBuilder builder;
    PartGraphPackage pkg =
        builder.AddJointInstance(parts::JointSquareWithTwoSockets, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f1")
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f2")
            .Connect("f1", 0, "j0", 0)
            .Connect("f2", 0, "j0", 1)
            .Build();

    PartGraph &g = pkg.part_graph;

    auto &j0 = std::get<JointInstance>(g.at(pkg.id_to_part_graph_id.at("j0")));
    j0.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
    auto &f1 =
        std::get<FragmentInstance>(g.at(pkg.id_to_part_graph_id.at("f1")));
    auto &f2 =
        std::get<FragmentInstance>(g.at(pkg.id_to_part_graph_id.at("f2")));

    auto result = position_part_graph_from_first_added(g);

    REQUIRE_THAT(f1.GetSocketWorldPosition(0),
                 EqualsVector2f(j0.GetSocketWorldPosition(0), 0.01f));

    REQUIRE_THAT(f2.GetSocketWorldPosition(0),
                 EqualsVector2f(j0.GetSocketWorldPosition(1), 0.01f));
  }

  SECTION("Does not infinite-loop on a cyclic graph") {
    PartGraphBuilder builder;
    PartGraphPackage pkg =
        builder.AddJointInstance(parts::JointSquareWithTwoSockets, "j0")
            .AddFragmentInstance(parts::FragmentRectangleWithTwoSockets, "f1")
            .Connect("f1", 0, "j0", 0)
            .Connect("f1", 1, "j0",
                     1) // creates cycle-like revisit opportunities
            .Build();

    PartGraph &g = pkg.part_graph;
    auto &j0 = std::get<JointInstance>(g.at(pkg.id_to_part_graph_id.at("j0")));
    j0.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);

    auto result = position_part_graph_from_first_added(g);
    REQUIRE(result.has_value());
  }
}

TEST_CASE("calculate_composite_box tests") {

  sf::FloatRect compounding_box{};
  REQUIRE(compounding_box.position.x == 0);
  REQUIRE(compounding_box.position.y == 0);
  REQUIRE(compounding_box.size.x == 0);
  REQUIRE(compounding_box.size.y == 0);

  SECTION(" next box is added to compounding_box at origin and no size") {
    sf::FloatRect next_box{{50, 50}, {50, 50}};
    calculate_composite_box(compounding_box, next_box);

    // this box is to the right of the compounding_box at both x and y so
    // position of compounding_box should not change
    REQUIRE(compounding_box.position.x == 0);
    REQUIRE(compounding_box.position.y == 0);
    REQUIRE(compounding_box.size.x == 100);
    REQUIRE(compounding_box.size.y == 100);
  }

  SECTION("the next box has left edge further left ") {
    compounding_box.position = {0, 0};
    compounding_box.size = {100, 100};
    sf::FloatRect next_box{{-25, 10}, {50, 50}};
    calculate_composite_box(compounding_box, next_box);

    REQUIRE(compounding_box.position.x == -25);
    REQUIRE(compounding_box.position.y == 0);
    REQUIRE(compounding_box.size.x == 125);
    REQUIRE(compounding_box.size.y == 100);
  }

  SECTION("the next box has a top edge further up") {

    compounding_box.position = {0, 0};
    compounding_box.size = {100, 100};
    sf::FloatRect next_box{{10, -30}, {50, 50}};
    calculate_composite_box(compounding_box, next_box);

    REQUIRE(compounding_box.position.x == 0);
    REQUIRE(compounding_box.position.y == -30);
    REQUIRE(compounding_box.size.x == 100);
    REQUIRE(compounding_box.size.y == 130);
  }

  SECTION("the next box is bigger in all dimensions") {

    compounding_box.position = {0, 0};
    compounding_box.size = {60, 60};
    sf::FloatRect next_box{{-25, -30}, {130, 130}};
    calculate_composite_box(compounding_box, next_box);

    REQUIRE(compounding_box.position.x == -25);
    REQUIRE(compounding_box.position.y == -30);
    REQUIRE(compounding_box.size.x == 130);
    REQUIRE(compounding_box.size.y == 130);
  }

  SECTION("10 boxes are added sequentially to the compounding box") {
    compounding_box.position = {0, 0};
    compounding_box.size = {0, 0};
    for (int i = 0; i < 10; ++i) {
      sf::FloatRect next_box{
          {static_cast<float>(i * 10), static_cast<float>(i * 10)},
          {10.f, 10.f}};
      calculate_composite_box(compounding_box, next_box);
    }
    REQUIRE(compounding_box.position.x == 0);
    REQUIRE(compounding_box.position.y == 0);
    REQUIRE(compounding_box.size.x == 100);
    REQUIRE(compounding_box.size.y == 100);
  }
}
TEST_CASE("calculate_outer_box tests") {

  // set up a fragment wiht a single triangle in the front view
  Fragment fragment;
  sf::VertexArray &front_array =
      fragment.positioning_views[ViewDirection::Front];
  front_array.setPrimitiveType(sf::PrimitiveType::Triangles);
  front_array.append(sf::Vertex(sf::Vector2f(0.f, 0.f)));
  front_array.append(sf::Vertex(sf::Vector2f(30.f, 0.f)));
  front_array.append(sf::Vertex(sf::Vector2f(15.f, 30.f)));

  Fragment fragment2;
  sf::VertexArray &front_array2 =
      fragment2.positioning_views[ViewDirection::Front];
  front_array2.setPrimitiveType(sf::PrimitiveType::Triangles);
  front_array2.append(sf::Vertex(sf::Vector2f(10.f, 10.f)));
  front_array2.append(sf::Vertex(sf::Vector2f(40.f, 10.f)));
  front_array2.append(sf::Vertex(sf::Vector2f(25.f, 40.f)));

  // large fragment
  Fragment fragment_large;
  sf::VertexArray &front_array_large =
      fragment_large.positioning_views[ViewDirection::Front];
  front_array_large.setPrimitiveType(sf::PrimitiveType::Triangles);
  front_array_large.append(sf::Vertex(sf::Vector2f(-100.f, -100.f)));
  front_array_large.append(sf::Vertex(sf::Vector2f(150.f, -100.f)));
  front_array_large.append(sf::Vertex(sf::Vector2f(25.f, 150.f)));

  SECTION("PartGraph has only one part and no subgraph provided, does not "
          "exceed minimum box size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance{0, fragment};
    part_graph.emplace(fragment_instance.GetId(), fragment_instance);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph

    REQUIRE(outer_box.size.x == 200.f);
    REQUIRE(outer_box.size.y == 200.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has only one part and no subgraph provided, exceeds "
          "minimum size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance{0, fragment_large};
    part_graph.emplace(fragment_instance.GetId(), fragment_instance);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph
    // The outer box should be the same as the fragment's bounding box since
    // it exceeds the minimum size
    REQUIRE(outer_box.size.x == 250.f);
    REQUIRE(outer_box.size.y == 250.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has only one part, no subgraph provided, and local "
          "transform applied, does not exceed minimum box size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance{0, fragment};
    // apply a local transform to the fragment instance
    fragment_instance.move({10.f, 10.f});
    part_graph.emplace(fragment_instance.GetId(), fragment_instance);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph

    REQUIRE(outer_box.size.x == 200.f);
    REQUIRE(outer_box.size.y == 200.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has only one part, no subgraph provided, and local "
          "transform applied, exceeds minimum box size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance{0, fragment_large};
    // apply a local transform to the fragment instance
    fragment_instance.move({10.f, 10.f});
    part_graph.emplace(fragment_instance.GetId(), fragment_instance);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph
    // The outer box should be the same as the fragment's bounding box since
    // it exceeds the minimum size
    REQUIRE(outer_box.size.x == 260.f);
    REQUIRE(outer_box.size.y == 260.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has multiple parts, no subgraph provided and transforms "
          "applied, does not exceed minimum box size") {
    PartGraph part_graph;
    FragmentInstance fragment_instance1{0, fragment};
    fragment_instance1.move({10.f, 10.f});
    part_graph.emplace(fragment_instance1.GetId(), fragment_instance1);
    FragmentInstance fragment_instance2{1, fragment2};
    fragment_instance2.move({-5.f, -5.f});
    part_graph.emplace(fragment_instance2.GetId(), fragment_instance2);
    sf::FloatRect outer_box =
        calculate_outer_box(part_graph, SubGraph{}); // empty subgraph

    REQUIRE(outer_box.size.x == 200.f);
    REQUIRE(outer_box.size.y == 200.f);
    REQUIRE(outer_box.position.x == -100.f);
    REQUIRE(outer_box.position.y == -100.f);
  }

  SECTION("PartGraph has multiple parts, subgraph provided and transforms "
          "applied") {
    PartGraph part_graph;
    FragmentInstance fragment_instance1{0, fragment};

    fragment_instance1.move({10.f, 10.f});
    part_graph.emplace(fragment_instance1.GetId(), fragment_instance1);
    FragmentInstance fragment_instance2{1, fragment2};
    fragment_instance2.move({-5.f, -5.f});
    part_graph.emplace(fragment_instance2.GetId(), fragment_instance2);
    SubGraph sub_graph{0}; // only include the first part
    sf::FloatRect outer_box = calculate_outer_box(part_graph, sub_graph);

    // because a subgraph has been provided, the minimum box size in the
    // function is not applied
    REQUIRE(outer_box.size.x == 30.f);
    REQUIRE(outer_box.size.y == 30.f);
    REQUIRE(outer_box.position.x == 10.f);
    REQUIRE(outer_box.position.y == 10.f);
  }
}

} // namespace steamrot::tests
