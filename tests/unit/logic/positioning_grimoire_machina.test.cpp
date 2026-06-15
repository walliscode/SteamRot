/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for positioning grimoire machina free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include "Vector2fEqualsMatcher.h"
#include "grimoire_machina_test_helpers.h"
#include <SFML/Graphics/Transform.hpp>
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
    joint.origin = {5.f, 5.f}; // set origin to (5,5)
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
        instance.transform.transformPoint(instance.joint->origin);

    REQUIRE_THAT(actual_position,
                 steamrot::tests::EqualsVector2f(expected_position));
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
        // Assert: sockets should be at radius distance from origin at 0° and
        // 90°
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
        // Assert: sockets should be at radius distance from origin at 0° and
        // 90°
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
        // Assert: sockets should be at radius distance from origin at 0° and
        // 180°
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
        // Assert: sockets should be at radius distance from origin at 0° and
        // 180°
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
        // Assert: sockets should be at radius distance from origin at 0°, 45°,
        // and 90°
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
        // Assert: sockets should be at radius distance from origin at 0°, 90°,
        // and 180°
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

    // Assert: sockets should be at radius distance from origin at 0°, 135°, and
    // 270°
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
} // namespace steamrot::tests
