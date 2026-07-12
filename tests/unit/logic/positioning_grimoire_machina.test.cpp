/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning grimoire machina free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "Fragment.h"
#include "Joint.h"
#include "MachinaFormScaffold.h"
#include "Vector2fEqualsMatcher.h"
#include "ViewDirection.h"
#include "fragment_library.h"
#include "grimoire_machina_test_helpers.h"
#include "joint_library.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace steamrot::logic::positioning::grimoire_machina;

TEST_CASE("position_first_part_of_machina_form tests",
          "[positioning_grimoire_machina]") {
  // Arrange
  steamrot::PartGraph parts;

  SECTION("Does not throw when parts is empty") {
    // Act & Assert
    REQUIRE_NOTHROW(position_first_part_of_machina_form_scaffold(parts));
  }

  SECTION("Does not throw when Fragment has no sockets") {
    // Arrange
    steamrot::Fragment fragment{}; // empty fragment
    steamrot::FragmentInstance fragment_instance{
        &fragment}; // instance of that fragment
    fragment_instance.id = 0;

    parts.emplace(fragment_instance.id, fragment_instance); // add to parts map

    REQUIRE(parts.size() == 1); // sanity check
    // Act & Assert
    REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
                        position_first_part_of_machina_form_scaffold(parts));
  }

  SECTION("Positions centre of first Fragmentinstance's FRONT view at 0,0") {
    // Arrange
    auto fragment = steamrot::tests::MakeFragmentWithFrontView();
    steamrot::FragmentInstance fragment_instance{&fragment};
    fragment_instance.id = 0;
    parts.emplace(fragment_instance.id, fragment_instance);
    REQUIRE(parts.size() == 1); // sanity check
    // pull out reference to the Fragmentinstance we just added so we can check
    // its transform after
    steamrot::FragmentInstance &instance =
        std::get<steamrot::FragmentInstance>(parts.at(0));

    // Act
    steamrot::logic::positioning::grimoire_machina::
        position_first_part_of_machina_form_scaffold(parts);

    // we expect the center to be translated to the origin
    sf::Vector2f expected_position{0.f, 0.f};

    // centre of box around triangle is at {15,15}
    sf::Vector2f actual_position =
        instance.transform.transformPoint({15.f, 15.f});

    REQUIRE_THAT(actual_position,
                 steamrot::tests::EqualsVector2f(expected_position));
  }

  SECTION("Positions origin of first JointInstance at 0,0") {
    // Arrange
    auto joint = steamrot::tests::MakeJointWithFrontView();
    joint.socket_pivot = {5.f, 5.f}; // set origin to (5,5)
    steamrot::JointInstance joint_instance{&joint};
    joint_instance.id = 0;
    parts.emplace(joint_instance.id, joint_instance);
    REQUIRE(parts.size() == 1); // sanity check
    // pull out reference to the JointInstance we just added so we can check
    // its transform after
    steamrot::JointInstance &instance =
        std::get<steamrot::JointInstance>(parts.at(0));
    // Act
    steamrot::logic::positioning::grimoire_machina::
        position_first_part_of_machina_form_scaffold(parts);

    // we expect the joint origin to be translated to {0,0}
    sf::Vector2f expected_position{0.f, 0.f};

    sf::Vector2f actual_position =
        instance.transform.transformPoint(instance.joint->socket_pivot);

    REQUIRE_THAT(actual_position,
                 steamrot::tests::EqualsVector2f(expected_position));
  }
}

TEST_CASE("maximise_joint_socket_spread tests",
          "[positioning_grimoire_machina]") {
  // Arrange
  Joint joint;
  joint.socket_pivot = {0.f, 0.f};
  SocketConfig &config = joint.socket_config;

  SECTION("Does not throw with zero sockets") {

    // Arrange
    config.socket_count = 0;
    JointInstance joint_instance{&joint};
    // Act & Assert
    REQUIRE_NOTHROW(maximise_joint_socket_spread(joint_instance));
  }

  SECTION("Joint with 2 sockets") {
    config.socket_count = 2;
    JointInstance joint_instance{&joint};
    REQUIRE(joint_instance.sockets.size() == 2); // sanity check

    SECTION("90° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f; // arc from 0° to 90°
      config.radius = 10.f;
      maximise_joint_socket_spread(joint_instance);
      // Assert: sockets should be at radius distance from origin at 0° and 90°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{0.f, 10.f};
      REQUIRE_THAT(
          joint_instance.sockets.at(0).local_position,
          steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(
          joint_instance.sockets.at(1).local_position,
          steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
    }

    SECTION("180° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f; // arc from 0° to 180°
      config.radius = 10.f;
      maximise_joint_socket_spread(joint_instance);
      // Assert: sockets should be at radius distance from origin at 0° and 180°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{-10.f, 0.f};
      REQUIRE_THAT(
          joint_instance.sockets.at(0).local_position,
          steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(
          joint_instance.sockets.at(1).local_position,
          steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
    }

    SECTION("270° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 270.f; // arc from 0° to 270°
      config.radius = 10.f;
      maximise_joint_socket_spread(joint_instance);
      // Assert: sockets should be at radius distance from origin at 0° and 270°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{0.f, -10.f};
      REQUIRE_THAT(
          joint_instance.sockets.at(0).local_position,
          steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(
          joint_instance.sockets.at(1).local_position,
          steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
    }
  }

  SECTION("Joint with 3 sockets") {
    config.socket_count = 3;
    JointInstance joint_instance{&joint};
    REQUIRE(joint_instance.sockets.size() == 3); // sanity check
    //
    SECTION("90° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f; // arc from 0° to 90°
      config.radius = 10.f;
      maximise_joint_socket_spread(joint_instance);
      // Assert: sockets should be at radius distance from origin at 0°, 45°,
      // and 90°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{7.071f, 7.071f}; // (10 * cos(45°), 10 *
                                                        // sin(45°))
      sf::Vector2f expected_position_2{0.f, 10.f};
      REQUIRE_THAT(
          joint_instance.sockets.at(0).local_position,
          steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(
          joint_instance.sockets.at(1).local_position,
          steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
      REQUIRE_THAT(
          joint_instance.sockets.at(2).local_position,
          steamrot::tests::EqualsVector2f(expected_position_2, 0.001f));
    }

    SECTION("180° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f; // arc from 0° to 180°
      config.radius = 10.f;
      maximise_joint_socket_spread(joint_instance);
      // Assert: sockets should be at radius distance from origin at 0°, 90°,
      // and 180°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{0.f, 10.f};
      sf::Vector2f expected_position_2{-10.f, 0.f};
      REQUIRE_THAT(
          joint_instance.sockets.at(0).local_position,
          steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(
          joint_instance.sockets.at(1).local_position,
          steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
      REQUIRE_THAT(
          joint_instance.sockets.at(2).local_position,
          steamrot::tests::EqualsVector2f(expected_position_2, 0.001f));
    }

    SECTION("270° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 270.f; // arc from 0° to 270°
      config.radius = 10.f;
      maximise_joint_socket_spread(joint_instance);
      // Assert: sockets should be at radius distance from origin at 0°, 135°,
      // and 270°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{-7.071f, 7.071f}; // (10 * cos(135°), 10
                                                         // * sin(135°))
      sf::Vector2f expected_position_2{0.f, -10.f};
      REQUIRE_THAT(
          joint_instance.sockets.at(0).local_position,
          steamrot::tests::EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(
          joint_instance.sockets.at(1).local_position,
          steamrot::tests::EqualsVector2f(expected_position_1, 0.001f));
      REQUIRE_THAT(
          joint_instance.sockets.at(2).local_position,
          steamrot::tests::EqualsVector2f(expected_position_2, 0.001f));
    }
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

TEST_CASE("align_fragment_onto_joint_socket tests") {

  FragmentInstance fragment_instance{&parts::FragmentRectangleWithTwoSockets};
  fragment_instance.transform = sf::Transform::Identity; // reset transform
  JointInstance joint_instance{&parts::JointSquareWithTwoSockets};
  joint_instance.transform = sf::Transform::Identity; // reset transform

  // maximise the joint socket spread so we know where the sockets are
  maximise_joint_socket_spread(joint_instance);
  // check the positions of the sockets
  REQUIRE_THAT(joint_instance.sockets.at(0).local_position,
               steamrot::tests::EqualsVector2f({23.f, 10.f}, 0.001f));
  REQUIRE_THAT(joint_instance.sockets.at(1).local_position,
               steamrot::tests::EqualsVector2f({10.f, 23.f}, 0.001f));

  SECTION("Does not throw if fragmnet socket id is invalid") {
    REQUIRE_NOTHROW(align_fragment_onto_joint_socket(fragment_instance, 99,
                                                     joint_instance, 0));
  }
  SECTION("Does not throw if joint socket id is invalid") {
    REQUIRE_NOTHROW(align_fragment_onto_joint_socket(fragment_instance, 0,
                                                     joint_instance, 99));
  }

  SECTION("Does not align if no connection between fragment and joint") {
    // Act
    align_fragment_onto_joint_socket(fragment_instance, 0, joint_instance, 0);
    // Assert: fragment should not be aligned to joint socket
    REQUIRE_THAT(fragment_instance.transform.transformPoint(
                     fragment_instance.sockets.at(0).local_position),
                 !steamrot::tests::EqualsVector2f(
                     joint_instance.transform.transformPoint(
                         joint_instance.sockets.at(0).local_position),
                     0.001f));
  }

  SECTION("FragmentInstance sockets are correctly translated and rotated"
          " to align with JointInstance sockets") {

    // add a connection between the fragment socket and the joint socket
    fragment_instance.sockets.at(0).connected_to = SocketConnection{
        joint_instance.id, 0}; // fragment socket 0 connected to joint socket 0
    fragment_instance.sockets.at(0).connection_state =
        SocketConnectionState::Connected;
    // add a connection between the joint socket and the fragment socket
    joint_instance.sockets.at(0).connected_to =
        SocketConnection{fragment_instance.id,
                         0}; // joint socket 0 connected to fragment socket 0
    joint_instance.sockets.at(0).connection_state =
        SocketConnectionState::Connected;

    // Act
    align_fragment_onto_joint_socket(fragment_instance, 0, joint_instance, 0);

    // Assert: fragment socket should be at the same position as joint socket
    REQUIRE_THAT(fragment_instance.transform.transformPoint(
                     fragment_instance.sockets.at(0).local_position),
                 EqualsVector2f({23, 10}, 0.001f));

    // so the joint socket is at {23,10} with an origin of {10,10}, so the
    // normalised vector from the origin to the socket is {13,0} normalized =
    // {1,0}. The fragment socket alignment vector is also {1,0} so no rotataion
    // applied
    REQUIRE_THAT(fragment_instance.transform.transformPoint(
                     fragment_instance.sockets.at(1).local_position),
                 EqualsVector2f({73.f, 10.f}, 0.001f));
  }

  SECTION("FragmentInstance is correctly translated and rotated to align with "
          "JointInstance") {

    // add a connection between the fragment socket and the joint socket
    fragment_instance.sockets.at(0).connected_to = SocketConnection{
        joint_instance.id, 1}; // fragment socket 0 connected to joint socket 1
    fragment_instance.sockets.at(0).connection_state =
        SocketConnectionState::Connected;

    // add a connection between the joint socket and the fragment socket
    joint_instance.sockets.at(1).connected_to =
        SocketConnection{fragment_instance.id,
                         0}; // joint socket 1 connected to fragment socket 0
    joint_instance.sockets.at(1).connection_state =
        SocketConnectionState::Connected;

    // Act
    align_fragment_onto_joint_socket(fragment_instance, 0, joint_instance, 1);

    // Assert: fragment should be aligned to joint socket
    REQUIRE_THAT(fragment_instance.transform.transformPoint(
                     fragment_instance.sockets.at(0).local_position),
                 EqualsVector2f({10.f, 23.f}, 0.001f));

    // so the joint socket is at {10,23} with an origin of {10,10}, so the
    // alignment vector from the origin to the socket is {0,13} normalized =
    // {0,1}. The fragment socket alignment vector is {1,0} so a rotation of 90°
    // should be applied
    REQUIRE_THAT(fragment_instance.transform.transformPoint(
                     fragment_instance.sockets.at(1).local_position),
                 EqualsVector2f({10.f, 73.f}, 0.001f));
  }
}
TEST_CASE("compute_socket_local_positions_even_spread tests",
          "[positioning_grimoire_machina]") {
  // Arrange
  steamrot::SocketConfig config;
  sf::Vector2f origin{0.f, 0.f};
  std::vector<sf::Vector2f> local_positions; // prepare vector for 3 sockets

  SECTION("Does not throw with zero sockets") {

    // Arrange
    config.socket_count = 0;
    // Act & Assert
    REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
                        compute_socket_local_positions_even_spread(
                            config, origin, local_positions));
  }
  SECTION("Clears and resizes local_positions to socket_count") {
    // Arrange
    config.socket_count = 3;
    local_positions = {{1.f, 1.f},
                       {2.f, 2.f},
                       {3.f, 3.f},
                       {4.f, 4.f}}; // start with 4 positions
    // Act
    REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
                        compute_socket_local_positions_even_spread(
                            config, origin, local_positions));
    // Assert: local_positions should be cleared and resized to 3
    REQUIRE(local_positions.size() == 3);
  }
  SECTION("One socket tests") {
    // Arrange
    config.socket_count = 1;

    SECTION("90° arc") {

      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f; // arc from 0° to 90°

      SECTION("radius 0") {
        config.radius = 0.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: socket should be at origin
        REQUIRE_THAT(local_positions[0],
                     steamrot::tests::EqualsVector2f(origin, 0.001f));
      }
      SECTION("radius 10") {
        config.radius = 10.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);

        // Assert: socket should be at radius distance from origin at 45°
        sf::Vector2f expected_position{
            7.071f, 7.071f}; // (10 * cos(45°), 10 * sin(45°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position, 0.001f));
      }
      SECTION("radius 20") {
        config.radius = 20.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: socket should be at radius distance from origin at 45°
        sf::Vector2f expected_position{
            14.142f, 14.142f}; // (20 * cos(45°), 20 * sin(45°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position, 0.001f));
      }
    }
    SECTION("180° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f; // arc from 0° to 180°
      //
      SECTION("radius 10") {
        config.radius = 10.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: socket should be at radius distance from origin at 90°
        sf::Vector2f expected_position{0.f,
                                       10.f}; // (10 * cos(90°), 10 * sin(90°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position, 0.001f));
      }
      SECTION("radius 20") {
        config.radius = 20.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: socket should be at radius distance from origin at 90°
        sf::Vector2f expected_position{0.f,
                                       20.f}; // (20 * cos(90°), 20 * sin(90°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position, 0.001f));
      }
    }
  }

  SECTION("Two socket tests") {
    // arrange
    config.socket_count = 2;

    SECTION("90° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f; // arc from 0° to 90°

      SECTION("radius 0") {
        config.radius = 0.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: sockets should be at origin
        REQUIRE_THAT(local_positions[0],
                     steamrot::tests::EqualsVector2f(origin, 0.001f));
        REQUIRE_THAT(local_positions[1],
                     steamrot::tests::EqualsVector2f(origin, 0.001f));
      }
      SECTION("radius 10") {
        config.radius = 10.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: sockets should be at radius distance from origin at 0°
        // and 90°
        sf::Vector2f expected_position_0{10.f,
                                         0.f}; // (10 * cos(0°), 10 * sin(0°))
        sf::Vector2f expected_position_1{
            0.f, 10.f}; // (10 * cos(90°), 10 * sin(90°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position_0, 0.001f));
        REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
                                             expected_position_1, 0.001f));
      }
      SECTION("radius 20") {
        config.radius = 20.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: sockets should be at radius distance from origin at 0°
        // and 90°
        sf::Vector2f expected_position_0{20.f,
                                         0.f}; // (20 * cos(0°), 20 * sin(0°))
        sf::Vector2f expected_position_1{
            0.f, 20.f}; // (20 * cos(90°), 20 * sin(90°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position_0, 0.001f));
        REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
                                             expected_position_1, 0.001f));
      }
    }
    SECTION("180° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f; // arc from 0° to 180°
      SECTION("radius 10") {
        config.radius = 10.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: sockets should be at radius distance from origin at 0°
        // and 180°
        sf::Vector2f expected_position_0{10.f,
                                         0.f}; // (10 * cos(0°), 10 * sin(0°))
        sf::Vector2f expected_position_1{
            -10.f, 0.f}; // (10 * cos(180°), 10 * sin(180°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position_0, 0.001f));
        REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
                                             expected_position_1, 0.001f));
      }
      SECTION("radius 20") {
        config.radius = 20.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: sockets should be at radius distance from origin at 0°
        // and 180°
        sf::Vector2f expected_position_0{20.f,
                                         0.f}; // (20 * cos(0°), 20 * sin(0°))
        sf::Vector2f expected_position_1{
            -20.f, 0.f}; // (20 * cos(180°), 20 * sin(180°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position_0, 0.001f));
        REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
                                             expected_position_1, 0.001f));
      }
    }
  }
  SECTION("Three socket tests") {
    // arrange
    config.socket_count = 3;
    SECTION("90° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f; // arc from 0° to 90°
      SECTION("radius 10") {
        config.radius = 10.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: sockets should be at radius distance from origin at 0°,
        // 45°, and 90°
        sf::Vector2f expected_position_0{10.f,
                                         0.f}; // (10 * cos(0°), 10 * sin(0°))
        sf::Vector2f expected_position_1{
            7.071f, 7.071f}; // (10 * cos(45°), 10 * sin(45°))
        sf::Vector2f expected_position_2{
            0.f, 10.f}; // (10 * cos(90°), 10 * sin(90°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position_0, 0.001f));
        REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
                                             expected_position_1, 0.001f));
        REQUIRE_THAT(local_positions[2], steamrot::tests::EqualsVector2f(
                                             expected_position_2, 0.001f));
      }
    }
    SECTION("180° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f; // arc from 0° to 180°
      SECTION("radius 10") {
        config.radius = 10.f;
        compute_socket_local_positions_even_spread(config, origin,
                                                   local_positions);
        // Assert: sockets should be at radius distance from origin at 0°,
        // 90°, and 180°
        sf::Vector2f expected_position_0{10.f,
                                         0.f}; // (10 * cos(0°), 10 * sin(0°))
        sf::Vector2f expected_position_1{
            0.f, 10.f}; // (10 * cos(90°), 10 * sin(90°))
        sf::Vector2f expected_position_2{
            -10.f, 0.f}; // (10 * cos(180°), 10 * sin(180°))
        REQUIRE_THAT(local_positions[0], steamrot::tests::EqualsVector2f(
                                             expected_position_0, 0.001f));
        REQUIRE_THAT(local_positions[1], steamrot::tests::EqualsVector2f(
                                             expected_position_1, 0.001f));
        REQUIRE_THAT(local_positions[2], steamrot::tests::EqualsVector2f(
                                             expected_position_2, 0.001f));
      }
    }
  }
}

TEST_CASE("initialize_joint_socket_positions tests",
          "[unit][positioning_grimoire_machina]") {

  SECTION("Does not throw when joint pointer is null") {
    steamrot::JointInstance instance{nullptr};
    REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
                        initialize_joint_socket_positions(instance));
  }

  SECTION("Populates socket positions from SocketConfig even spread") {

    steamrot::Joint joint;
    joint.socket_config.socket_count = 3;
    joint.socket_config.radius = 10.f;
    joint.socket_config.rotation_arc_min = 0.f;
    joint.socket_config.rotation_arc_max = 270.f;

    steamrot::JointInstance instance{&joint};
    steamrot::logic::positioning::grimoire_machina::
        initialize_joint_socket_positions(instance);

    // Assert: sockets should be at radius distance from origin at 0°, 135°,
    // and 270°
    REQUIRE_THAT(instance.sockets.at(0).local_position,
                 EqualsVector2f({10.f, 0.f}, 0.001f));
    REQUIRE_THAT(instance.sockets.at(1).local_position,
                 EqualsVector2f({-7.071f, 7.071f}, 0.001f));
    REQUIRE_THAT(instance.sockets.at(2).local_position,
                 EqualsVector2f({0.f, -10.f}, 0.001f));
  }

  SECTION("Does nothing when socket_count is zero") {
    steamrot::Joint joint;
    joint.socket_config.socket_count = 0;

    steamrot::JointInstance instance{&joint};
    REQUIRE_NOTHROW(steamrot::logic::positioning::grimoire_machina::
                        initialize_joint_socket_positions(instance));
    REQUIRE(instance.sockets.empty());
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
    FragmentInstance fragment_instance{&fragment};
    fragment_instance.id = 0;
    part_graph.emplace(fragment_instance.id, fragment_instance);
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
    FragmentInstance fragment_instance{&fragment_large};
    fragment_instance.id = 0;
    part_graph.emplace(fragment_instance.id, fragment_instance);
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
    FragmentInstance fragment_instance{&fragment};
    fragment_instance.id = 0;
    // apply a local transform to the fragment instance
    fragment_instance.transform.translate({10.f, 10.f});
    part_graph.emplace(fragment_instance.id, fragment_instance);
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
    FragmentInstance fragment_instance{&fragment_large};
    fragment_instance.id = 0;
    // apply a local transform to the fragment instance
    fragment_instance.transform.translate({10.f, 10.f});
    part_graph.emplace(fragment_instance.id, fragment_instance);
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
    FragmentInstance fragment_instance1{&fragment};
    fragment_instance1.id = 0;
    fragment_instance1.transform.translate({10.f, 10.f});
    part_graph.emplace(fragment_instance1.id, fragment_instance1);
    FragmentInstance fragment_instance2{&fragment2};
    fragment_instance2.id = 1;
    fragment_instance2.transform.translate({-5.f, -5.f});
    part_graph.emplace(fragment_instance2.id, fragment_instance2);
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
    FragmentInstance fragment_instance1{&fragment};
    fragment_instance1.id = 0;
    fragment_instance1.transform.translate({10.f, 10.f});
    part_graph.emplace(fragment_instance1.id, fragment_instance1);
    FragmentInstance fragment_instance2{&fragment2};
    fragment_instance2.id = 1;
    fragment_instance2.transform.translate({-5.f, -5.f});
    part_graph.emplace(fragment_instance2.id, fragment_instance2);
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
