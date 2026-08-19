/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the JointInstance class and related free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointInstance.h"
#include "Vector2fEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("JointInstance::JointInstance tests", "[JointInstance]") {
  // Arrange
  Joint joint{};
  joint.socket_pivot = {0.f, 0.f};
  joint.socket_config.socket_count = 2;
  joint.socket_config.radius = 10.f;
  joint.socket_config.rotation_arc_min = 0.f;
  joint.socket_config.rotation_arc_max = 90.f;
  // Act
  JointInstance instance{1, joint};
  // Assert
  REQUIRE(instance.GetId() == 1);
  REQUIRE(instance.GetSocketPivotWorldPosition() ==
          instance.GetTransform().transformPoint(joint.socket_pivot));
  // The Sockets should be populated at construction with the correct number of
  // sockets based on the Joint's SocketConfig
  REQUIRE(instance.GetSocketCount() == joint.socket_config.socket_count);
}

TEST_CASE("maximise_joint_socket_spread tests",
          "[positioning_grimoire_machina]") {
  // Arrange
  JointInstance::Sockets sockets;
  sf::Vector2f pivot{0.f, 0.f};
  SocketConfig config;

  SECTION("Does not throw with zero sockets") {

    // Arrange
    config.socket_count = 0;

    // Act & Assert
    REQUIRE_NOTHROW(maximise_joint_socket_spread(sockets, pivot, config));
  }

  SECTION("Joint with 2 sockets") {
    config.socket_count = 2;
    sockets.emplace(0, JointSocketState{});
    sockets.emplace(1, JointSocketState{});
    REQUIRE(sockets.size() == 2); // sanity check
    //
    SECTION("90° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f; // arc from 0° to 90°
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      // Assert: sockets should be at radius distance from origin at 0° and 90°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{0.f, 10.f};
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f(expected_position_1, 0.001f));
    }

    SECTION("180° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f; // arc from 0° to 180°
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      // Assert: sockets should be at radius distance from origin at 0° and 180°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{-10.f, 0.f};
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f(expected_position_1, 0.001f));
    }

    SECTION("270° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 270.f; // arc from 0° to 270°
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      // Assert: sockets should be at radius distance from origin at 0° and 270°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{0.f, -10.f};
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f(expected_position_1, 0.001f));
    }
  }

  SECTION("Joint with 3 sockets") {
    config.socket_count = 3;

    sockets.emplace(0, JointSocketState{});
    sockets.emplace(1, JointSocketState{});
    sockets.emplace(2, JointSocketState{});
    REQUIRE(sockets.size() == 3); // sanity check
    //
    SECTION("90° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 90.f; // arc from 0° to 90°
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      // Assert: sockets should be at radius distance from origin at 0°, 45°,
      // and 90°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{7.071f,
                                       7.071f}; // (10 * cos(45°), 10* sin(45°))
      sf::Vector2f expected_position_2{0.f, 10.f};
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f(expected_position_1, 0.001f));
      REQUIRE_THAT(sockets.at(2).GetLocalPosition(),
                   EqualsVector2f(expected_position_2, 0.001f));
    }

    SECTION("180° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 180.f; // arc from 0° to 180°
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      // Assert: sockets should be at radius distance from origin at 0°, 90°,
      // and 180°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{0.f, 10.f};
      sf::Vector2f expected_position_2{-10.f, 0.f};
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f(expected_position_1, 0.001f));
      REQUIRE_THAT(sockets.at(2).GetLocalPosition(),
                   EqualsVector2f(expected_position_2, 0.001f));
    }

    SECTION("270° arc") {
      config.rotation_arc_min = 0.f;
      config.rotation_arc_max = 270.f; // arc from 0° to 270°
      config.radius = 10.f;
      maximise_joint_socket_spread(sockets, pivot, config);
      // Assert: sockets should be at radius distance from origin at 0°,135°,
      // and 270°
      sf::Vector2f expected_position_0{10.f, 0.f};
      sf::Vector2f expected_position_1{-7.071f, 7.071f}; // (10 * cos(135°),10
      // * sin(135°))
      sf::Vector2f expected_position_2{0.f, -10.f};
      REQUIRE_THAT(sockets.at(0).GetLocalPosition(),
                   EqualsVector2f(expected_position_0, 0.001f));
      REQUIRE_THAT(sockets.at(1).GetLocalPosition(),
                   EqualsVector2f(expected_position_1, 0.001f));
      REQUIRE_THAT(sockets.at(2).GetLocalPosition(),
                   EqualsVector2f(expected_position_2, 0.001f));
    }
  }
}
} // namespace steamrot::tests
