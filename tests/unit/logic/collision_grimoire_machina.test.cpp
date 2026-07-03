/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for collision checking of GrimoireMachina elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include "PartGraphBuilder.h"
#include "SocketState.h"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "fragment_library.h"
#include "joint_library.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace logic::collision::grimoire_machina;

/////////////////////////////////////////////////
/// Helpers
/////////////////////////////////////////////////
namespace {

/// @brief Manually reset proximity state on a SocketData.
///        Simulates what reset_socket_proximity_state() does at PartGraph
///        level.
void reset_socket(SocketState &s) {
  s.is_another_socket_near = false;
  s.is_ready_to_connect = false;
  s.distance_to_nearest_socket = std::nullopt;
  s.proximity_scale = std::nullopt;
}

} // namespace

TEST_CASE("check_socket_collisions(SocketData, SocketData) tests",
          "[unit][collision_grimoire_machina]") {

  // arrange
  SocketState socket_one_data{};
  SocketState socket_two_data{};

  SECTION("sockets do not collide when far apart") {
    // arrange
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({100.f, 100.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert
    REQUIRE(socket_one_data.connection_state ==
            SocketConnectionState::Available);
    REQUIRE(socket_one_data.is_mouse_over == false);
    REQUIRE(socket_one_data.is_another_socket_near == false);
    REQUIRE(socket_one_data.is_ready_to_connect == false);
    REQUIRE(socket_one_data.distance_to_nearest_socket == std::nullopt);
    REQUIRE(socket_one_data.proximity_scale == std::nullopt);

    REQUIRE(socket_two_data.connection_state ==
            SocketConnectionState::Available);
    REQUIRE(socket_two_data.is_mouse_over == false);
    REQUIRE(socket_two_data.is_another_socket_near == false);
    REQUIRE(socket_two_data.is_ready_to_connect == false);
    REQUIRE(socket_two_data.distance_to_nearest_socket == std::nullopt);
    REQUIRE(socket_two_data.proximity_scale == std::nullopt);
  }

  SECTION("is_another_socket_near is true when sockets are within proximity "
          "threshold") {
    // arrange
    // Socket one at world (0,0), socket two at world (5,5).
    // Distance = sqrt(5^2 + 5^2) = sqrt(50) ≈ 7.07, within proximity (10).
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({5.f, 5.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert
    REQUIRE(socket_one_data.is_another_socket_near == true);
    REQUIRE(socket_one_data.is_ready_to_connect == false);
    REQUIRE(socket_one_data.distance_to_nearest_socket.has_value());
    REQUIRE_THAT(socket_one_data.distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(7.07f, 0.01f));

    REQUIRE(socket_two_data.is_ready_to_connect == false);
    REQUIRE(socket_two_data.is_another_socket_near == true);
    REQUIRE(socket_two_data.distance_to_nearest_socket.has_value());
    REQUIRE_THAT(socket_two_data.distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(7.07f, 0.01f));
  }

  SECTION("is_another_socket_near is false when sockets are outside proximity "
          "threshold after being near") {
    // arrange — first tick: sockets near
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({5.f, 5.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert
    REQUIRE(socket_one_data.is_another_socket_near == true);
    REQUIRE(socket_one_data.is_ready_to_connect == false);
    REQUIRE(socket_one_data.distance_to_nearest_socket.has_value());
    REQUIRE_THAT(socket_one_data.distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(7.07f, 0.01f));

    REQUIRE(socket_two_data.is_ready_to_connect == false);
    REQUIRE(socket_two_data.is_another_socket_near == true);
    REQUIRE(socket_two_data.distance_to_nearest_socket.has_value());
    REQUIRE_THAT(socket_two_data.distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(7.07f, 0.01f));

    // reset state (at PartGraph level this is handled by
    // reset_socket_proximity_state)
    reset_socket(socket_one_data);
    reset_socket(socket_two_data);

    // arrange — second tick: sockets far apart
    socket_one_transform.translate({-100.f, -100.f});
    socket_two_transform.translate({100.f, 100.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert
    REQUIRE(socket_one_data.is_another_socket_near == false);
    REQUIRE(socket_one_data.is_ready_to_connect == false);
    REQUIRE(socket_two_data.is_another_socket_near == false);
    REQUIRE(socket_two_data.is_ready_to_connect == false);
  }

  SECTION("is_ready_to_connect is set to true when sockets are within "
          "connection threshold") {
    // arrange
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({1.f, 1.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert
    REQUIRE(socket_one_data.is_another_socket_near == true);
    REQUIRE(socket_one_data.is_ready_to_connect == true);
    REQUIRE(socket_two_data.is_another_socket_near == true);
    REQUIRE(socket_two_data.is_ready_to_connect == true);
  }

  SECTION("is_ready_to_connect is false when sockets were within connection "
          "threshold but then moved apart") {
    // arrange — first tick: sockets close enough to connect
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({1.f, 1.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert
    REQUIRE(socket_one_data.is_another_socket_near == true);
    REQUIRE(socket_one_data.is_ready_to_connect == true);
    REQUIRE(socket_two_data.is_another_socket_near == true);
    REQUIRE(socket_two_data.is_ready_to_connect == true);

    // reset state (at PartGraph level this is handled by
    // reset_socket_proximity_state)
    reset_socket(socket_one_data);
    reset_socket(socket_two_data);

    // arrange — second tick: sockets far apart
    socket_one_transform.translate({-100.f, -100.f});
    socket_two_transform.translate({100.f, 100.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert
    REQUIRE(socket_one_data.is_another_socket_near == false);
    REQUIRE(socket_one_data.is_ready_to_connect == false);
    REQUIRE(socket_two_data.is_another_socket_near == false);
    REQUIRE(socket_two_data.is_ready_to_connect == false);
  }

  SECTION("proximity_scale is set and non-zero when sockets are within "
          "proximity threshold but not ready to connect") {
    // arrange: distance ≈ 7.07 (within proximity 10, outside connection 2.5)
    // Expected scale: (10.0 - 7.07) / (10.0 - 2.5) * 255 ≈ 99
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({5.f, 5.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert: proximity_scale should be set and match the expected value
    REQUIRE(socket_one_data.proximity_scale.has_value());
    REQUIRE(socket_one_data.proximity_scale.value() > 0);
    REQUIRE(socket_one_data.proximity_scale.value() < 255);
    // distance ≈ 7.07 → scale ≈ 99 (allow ±2 for floating-point rounding)
    REQUIRE(socket_one_data.proximity_scale.value() >= 97);
    REQUIRE(socket_one_data.proximity_scale.value() <= 101);
    REQUIRE(socket_two_data.proximity_scale.has_value());
    REQUIRE(socket_two_data.proximity_scale.value() > 0);
    REQUIRE(socket_two_data.proximity_scale.value() < 255);
    REQUIRE(socket_two_data.proximity_scale.value() >= 97);
    REQUIRE(socket_two_data.proximity_scale.value() <= 101);
  }

  SECTION(
      "proximity_scale is 255 when sockets are within connection threshold") {
    // arrange: distance ≈ 1.41 (within connection threshold 2.5)
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({1.f, 1.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert: at or inside connection threshold → scale clamped to 255
    REQUIRE(socket_one_data.proximity_scale.has_value());
    REQUIRE(socket_one_data.proximity_scale.value() == 255);
    REQUIRE(socket_two_data.proximity_scale.has_value());
    REQUIRE(socket_two_data.proximity_scale.value() == 255);
  }

  SECTION("proximity_scale is nullopt when sockets are far apart") {
    // arrange: far outside proximity threshold
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({100.f, 100.f});
    // act
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    // assert
    REQUIRE(socket_one_data.proximity_scale == std::nullopt);
    REQUIRE(socket_two_data.proximity_scale == std::nullopt);
  }

  SECTION(
      "proximity_scale is reset to nullopt after sockets move out of range") {
    // first tick: sockets near
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({5.f, 5.f});
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    REQUIRE(socket_one_data.proximity_scale.has_value());

    // reset state between ticks (simulates per-tick reset in PartGraph logic)
    reset_socket(socket_one_data);
    reset_socket(socket_two_data);

    // second tick: sockets far apart
    socket_one_transform.translate({-100.f, -100.f});
    socket_two_transform.translate({100.f, 100.f});
    check_socket_collisions(socket_one_data, {0, 0}, socket_one_transform,
                            socket_two_data, {0, 0}, socket_two_transform);
    REQUIRE(socket_one_data.proximity_scale == std::nullopt);
    REQUIRE(socket_two_data.proximity_scale == std::nullopt);
  }
}

TEST_CASE("check_socket_collisions tests",
          "[unit][collision_grimoire_machina]") {

  // arrange
  // FragmentRectangleWithOneSocket has one socket at local (0, 5).
  FragmentInstance fragment_instance{&parts::FragmentRectangleWithOneSocket};
  FragmentSocketState &fragment_socket_state = fragment_instance.sockets.at(0);
  REQUIRE(fragment_instance.sockets.size() == 1);

  // JointSquareWithOneSocket has one socket; local position starts at (0, 0).
  JointInstance joint_instance{&parts::JointSquareWithOneSocket};
  JointSocketState &joint_socket_state = joint_instance.sockets.at(0);
  REQUIRE(joint_instance.sockets.size() == 1);
  joint_socket_state.local_position = {0.f, 0.f}; // set for easy testing

  SECTION("sockets do not collide when far apart") {
    // arrange
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({100.f, 100.f});
    // act
    check_socket_collisions(fragment_instance, joint_instance);
    // assert
    SocketState &socket_data = fragment_instance.sockets.at(0);
    REQUIRE(socket_data.connection_state ==
            fragment_socket_state.connection_state);
    REQUIRE(socket_data.is_mouse_over == false);
    REQUIRE(socket_data.is_another_socket_near == false);
    REQUIRE(socket_data.is_ready_to_connect == false);

    REQUIRE(joint_socket_state.connection_state ==
            joint_socket_state.connection_state);
    REQUIRE(joint_socket_state.is_mouse_over == false);
    REQUIRE(joint_socket_state.is_another_socket_near == false);
    REQUIRE(joint_socket_state.is_ready_to_connect == false);
  }

  SECTION("sockets do not collide if either SocketState is not Available",
          "[unit][collision_grimoire_machina]") {
    // arrange
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({1.f, 1.f}); // close enough to connect

    // Set one socket to Unavailable and the other to Hovered to test both
    // non-Available states.
    fragment_socket_state.connection_state =
        SocketConnectionState::Available;
    joint_socket_state.connection_state =
        SocketConnectionState::Connected;

    // act
    check_socket_collisions(fragment_instance, joint_instance);
    // assert: no collision should be detected because neither socket is
    // Available, so proximity state should remain false/false.
    REQUIRE(fragment_socket_state.is_another_socket_near == false);
    REQUIRE(fragment_socket_state.is_ready_to_connect == false);
    REQUIRE(joint_socket_state.is_another_socket_near == false);
    REQUIRE(joint_socket_state.is_ready_to_connect == false);
  }

  SECTION("is_another_socket_near is true when sockets are within proximity "
          "threshold") {
    // arrange
    // Fragment socket world pos: (0, 5). Joint socket at (5, 5) after translate.
    // Distance = sqrt((5-0)^2 + (5-5)^2) = 5, within proximity (10).
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({5.f, 5.f});
    // act
    check_socket_collisions(fragment_instance, joint_instance);
    // assert
    SocketState &socket_data = fragment_instance.sockets.at(0);
    REQUIRE(socket_data.is_another_socket_near == true);
    REQUIRE(socket_data.is_ready_to_connect == false);
    REQUIRE(joint_socket_state.is_another_socket_near == true);
    REQUIRE(joint_socket_state.is_ready_to_connect == false);
  }

  SECTION("is_another_socket_near is false when sockets were within proximity "
          "threshold but then moved apart") {
    // arrange — first tick: sockets near
    // Fragment socket world pos: (0, 5). Joint socket at (5, 5). Distance = 5.
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({5.f, 5.f});
    // act
    check_socket_collisions(fragment_instance, joint_instance);
    // assert
    SocketState &socket_data = fragment_instance.sockets.at(0);
    REQUIRE(socket_data.is_another_socket_near == true);
    REQUIRE(socket_data.is_ready_to_connect == false);
    REQUIRE(joint_socket_state.is_another_socket_near == true);
    REQUIRE(joint_socket_state.is_ready_to_connect == false);

    // reset state between ticks
    reset_socket(socket_data);
    reset_socket(joint_socket_state);

    // arrange — second tick: sockets far apart
    fragment_instance.transform.translate({-100.f, -100.f});
    joint_instance.transform.translate({100.f, 100.f});
    // act
    check_socket_collisions(fragment_instance, joint_instance);
    // assert
    REQUIRE(socket_data.is_another_socket_near == false);
    REQUIRE(socket_data.is_ready_to_connect == false);
    REQUIRE(joint_socket_state.is_another_socket_near == false);
    REQUIRE(joint_socket_state.is_ready_to_connect == false);
  }

  SECTION(
      "is_ready_to_connect is true when they are within connection threshold") {
    // arrange
    // Fragment socket world pos: (0, 5). Joint translate (0, 5) → socket at
    // (0, 5). Distance = 0, within connection threshold (2.5).
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({0.f, 5.f});
    // act
    check_socket_collisions(fragment_instance, joint_instance);
    // assert
    SocketState &socket_data = fragment_instance.sockets.at(0);
    REQUIRE(socket_data.is_another_socket_near == true);
    REQUIRE(socket_data.is_ready_to_connect == true);
    REQUIRE(joint_socket_state.is_another_socket_near == true);
    REQUIRE(joint_socket_state.is_ready_to_connect == true);
  }

  SECTION("is_ready_to_connect is false when sockets were within connection "
          "threshold but then moved apart") {
    // arrange — first tick: sockets close enough to connect
    // Fragment socket world: (0, 5). Joint translate (0, 5) → socket (0, 5).
    // Distance = 0.
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({0.f, 5.f});
    // act
    check_socket_collisions(fragment_instance, joint_instance);
    // assert
    SocketState &socket_data = fragment_instance.sockets.at(0);
    REQUIRE(socket_data.is_another_socket_near == true);
    REQUIRE(socket_data.is_ready_to_connect == true);
    REQUIRE(joint_socket_state.is_another_socket_near == true);
    REQUIRE(joint_socket_state.is_ready_to_connect == true);

    // reset state between ticks
    reset_socket(socket_data);
    reset_socket(joint_socket_state);

    // arrange — second tick: sockets far apart
    fragment_instance.transform.translate({-100.f, -100.f});
    joint_instance.transform.translate({100.f, 100.f});
    // act
    check_socket_collisions(fragment_instance, joint_instance);
    // assert
    REQUIRE(socket_data.is_another_socket_near == false);
    REQUIRE(socket_data.is_ready_to_connect == false);
    REQUIRE(joint_socket_state.is_another_socket_near == false);
    REQUIRE(joint_socket_state.is_ready_to_connect == false);
  }
}

TEST_CASE("check_socket_collisions(FragmentInstance, JointInstance) tests with "
          "multiple socket collisions",
          "[unit][collision_grimoire_machina]") {
  // arrange
  // FragmentRectangleWithTwoSockets: sockets at local (0, 5) and (50, 5).
  FragmentInstance fragment_instance{&parts::FragmentRectangleWithTwoSockets};
  FragmentSocketState &fragment_socket_data_one =
      fragment_instance.sockets.at(0);
  FragmentSocketState &fragment_socket_data_two =
      fragment_instance.sockets.at(1);

  // JointSquareWith2Sockets: two sockets; positions start at (0, 0).
  JointInstance joint_instance{&parts::JointSquareWith2Sockets};
  JointSocketState &joint_socket_data_one = joint_instance.sockets.at(0);
  JointSocketState &joint_socket_data_two = joint_instance.sockets.at(1);
  joint_socket_data_one.local_position = {0.f, 0.f};  // set for easy testing
  joint_socket_data_two.local_position = {10.f, 0.f}; // set for easy testing

  SECTION("sockets do not collide when far apart") {
    // arrange
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({100.f, 100.f});
    // act
    check_socket_collisions(fragment_instance, joint_instance);
    // assert
    for (const auto &[socket_id, socket_data] : fragment_instance.sockets) {
      REQUIRE(socket_data.connection_state ==
              SocketConnectionState::Available);
      REQUIRE(socket_data.is_mouse_over == false);
      REQUIRE(socket_data.is_another_socket_near == false);
      REQUIRE(socket_data.is_ready_to_connect == false);
    }
    for (const auto &[socket_id, socket_data] : joint_instance.sockets) {
      REQUIRE(socket_data.connection_state ==
              SocketConnectionState::Available);
      REQUIRE(socket_data.is_mouse_over == false);
      REQUIRE(socket_data.is_another_socket_near == false);
      REQUIRE(socket_data.is_ready_to_connect == false);
    }
  }

  SECTION("two sockets: most proximal joint socket wins per fragment socket") {
    // Arrange geometry:
    //   Fragment socket 0 at world (0,5), socket 1 at world (50,5).
    //   Joint socket 0 at local (0,4), socket 1 at local (0,13) —
    //   identity transforms on both instances.
    //
    // Distances:
    //   F[0](0,5) ↔ J[0](0,4)  = 1     → ready to connect
    //   F[0](0,5) ↔ J[1](0,13) = 8     → near only
    //   F[1](50,5) ↔ J[0](0,4) ≈ 50.01 → far (no update)
    //   F[1](50,5) ↔ J[1](0,13) ≈ 50.63 → far (no update)
    //
    // Expected: F[0] records J[0] (distance 1, ready) ignoring the farther
    // J[1]; J[0] is ready; J[1] is near (distance 8, from F[0]); F[1] has
    // no collision.
    joint_socket_data_one.local_position = {0.f, 4.f};
    joint_socket_data_two.local_position = {0, 13.f};

    check_socket_collisions(fragment_instance, joint_instance);

    // fragment socket 0: nearest is joint socket 0 (distance 1) — ready
    REQUIRE(fragment_socket_data_one.is_ready_to_connect == true);
    REQUIRE(fragment_socket_data_one.distance_to_nearest_socket.has_value());
    REQUIRE_THAT(fragment_socket_data_one.distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // fragment socket 1: no candidate in range
    REQUIRE(fragment_socket_data_two.is_another_socket_near == false);
    REQUIRE(fragment_socket_data_two.is_ready_to_connect == false);

    // joint socket 0: ready to connect (distance 1 from fragment socket 0)
    REQUIRE(joint_socket_data_one.is_ready_to_connect == true);
    REQUIRE_THAT(joint_socket_data_one.distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // joint socket 1: near only (distance 8 from fragment socket 0, which
    // is the best candidate it found — apply_if_better accepted it because
    // no closer candidate was recorded for joint socket 1)
    REQUIRE(joint_socket_data_two.is_another_socket_near == true);
    REQUIRE(joint_socket_data_two.is_ready_to_connect == false);
    REQUIRE_THAT(joint_socket_data_two.distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(8.f, 0.01f));
  }
}

TEST_CASE("check_socket_collisions(FragmentInstance, JointInstance) tests with "
          "three socket collisions",
          "[unit][collision_grimoire_machina]") {
  // arrange
  // FragmentRectangleWithThreeSockets: sockets at local (0,5), (50,5), (25,0).
  FragmentInstance fragment_instance{&parts::FragmentRectangleWithThreeSockets};
  REQUIRE(fragment_instance.sockets.size() == 3);

  // JointSquareWithThreeSockets: three sockets; positions start at (0, 0).
  JointInstance joint_instance{&parts::JointSquareWithThreeSockets};
  REQUIRE(joint_instance.sockets.size() == 3);

  // Override socket positions to known values for predictable geometry.
  // Fragment sockets at local (0,5), (50,5), (25,0).
  // Joint  sockets at local (1,0), (15,0), (25,0).
  joint_instance.sockets.at(0).local_position = {1.f, 0.f};
  joint_instance.sockets.at(1).local_position = {15.f, 0.f};
  joint_instance.sockets.at(2).local_position = {25.f, 0.f};
}

TEST_CASE("reset_socket_proximity_state(PartGraph) tests",
          "[unit][collision_grimoire_machina]") {
  SECTION("does not throw on an empty PartGraph") {
    PartGraph empty_map;
    REQUIRE_NOTHROW(reset_socket_proximity_state(empty_map));
  }

  SECTION("clears proximity state on all sockets in the PartGraph") {
    PartGraph part_graph =
        PartGraphBuilder{}
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f0")
            .AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .Build()
            .part_graph;

    // manually set proximity state on every socket
    for (auto &[id, variant] : part_graph) {
      auto visit = [](auto &instance) {
        for (auto &[socket_id, socket_data] : instance.sockets) {
          socket_data.is_another_socket_near = true;
          socket_data.is_ready_to_connect = true;
          socket_data.distance_to_nearest_socket = 3.f;
          socket_data.proximity_scale = uint8_t{200};
        }
      };
      if (auto *fi = std::get_if<FragmentInstance>(&variant))
        visit(*fi);
      else if (auto *ji = std::get_if<JointInstance>(&variant))
        visit(*ji);
    }

    // act
    reset_socket_proximity_state(part_graph);

    // assert — all sockets should be back to default
    for (auto &[id, variant] : part_graph) {
      auto check = [](const auto &instance) {
        for (const auto &[socket_id, socket_data] : instance.sockets) {
          REQUIRE(socket_data.is_another_socket_near == false);
          REQUIRE(socket_data.is_ready_to_connect == false);
          REQUIRE(socket_data.distance_to_nearest_socket == std::nullopt);
          REQUIRE(socket_data.proximity_scale == std::nullopt);
        }
      };
      if (const auto *fi = std::get_if<FragmentInstance>(&variant))
        check(*fi);
      else if (const auto *ji = std::get_if<JointInstance>(&variant))
        check(*ji);
    }
  }
}

TEST_CASE("check_socket_collisions(FragmentInstance, PartGraph) tests",
          "[unit][collision_grimoire_machina]") {
  PartGraphBuilder builder;

  // FragmentRectangleWithOneSocket: one socket at local (0, 5).
  // Identity transform → world socket at (0, 5).
  FragmentInstance fragment_instance{&parts::FragmentRectangleWithOneSocket};

  REQUIRE(fragment_instance.sockets.size() == 1);

  SECTION("does not throw on empty PartGraph") {
    PartGraph empty_map;
    REQUIRE_NOTHROW(
        check_socket_collisions(fragment_instance, empty_map));
    REQUIRE(fragment_instance.sockets.at(0).is_another_socket_near == false);
    REQUIRE(fragment_instance.sockets.at(0).is_ready_to_connect == false);
  }

  SECTION("no collision when joint socket is far away") {
    PartGraph part_graph =
        PartGraphBuilder{}
            .AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .Build()
            .part_graph;
    JointInstance &ji =
        std::get<JointInstance>(part_graph.begin()->second);
    ji.sockets.at(0).local_position = {0.f, 0.f};
    ji.transform.translate({100.f, 0.f}); // joint socket world (100,0)
    // Fragment socket world (0,5) — distance ≈ 100

    check_socket_collisions(fragment_instance, part_graph);

    REQUIRE(fragment_instance.sockets.at(0).is_another_socket_near == false);
    REQUIRE(fragment_instance.sockets.at(0).is_ready_to_connect == false);
    REQUIRE(ji.sockets.at(0).is_another_socket_near == false);
    REQUIRE(ji.sockets.at(0).is_ready_to_connect == false);
  }

  SECTION("is_another_socket_near is true when joint socket is within "
          "proximity") {
    PartGraph part_graph =
        PartGraphBuilder{}
            .AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .Build()
            .part_graph;
    JointInstance &ji =
        std::get<JointInstance>(part_graph.begin()->second);
    ji.sockets.at(0).local_position = {0.f, 0.f};
    // Joint socket world (5, 5). Fragment socket world (0, 5).
    // Distance = sqrt((5-0)^2 + (5-5)^2) = 5, within proximity (10).
    ji.transform.translate({5.f, 5.f});

    check_socket_collisions(fragment_instance, part_graph);

    REQUIRE(fragment_instance.sockets.at(0).is_another_socket_near == true);
    REQUIRE(fragment_instance.sockets.at(0).is_ready_to_connect == false);
    REQUIRE(ji.sockets.at(0).is_another_socket_near == true);
    REQUIRE(ji.sockets.at(0).is_ready_to_connect == false);
  }

  SECTION("is_ready_to_connect is true when joint socket is within connection "
          "threshold") {
    PartGraph part_graph =
        PartGraphBuilder{}
            .AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .Build()
            .part_graph;
    JointInstance &ji =
        std::get<JointInstance>(part_graph.begin()->second);
    ji.sockets.at(0).local_position = {0.f, 0.f};
    // Joint socket world (1, 4). Fragment socket world (0, 5).
    // Distance = sqrt((1-0)^2 + (4-5)^2) = sqrt(2) ≈ 1.41, within
    // connection threshold (2.5).
    ji.transform.translate({1.f, 4.f});

    check_socket_collisions(fragment_instance, part_graph);

    REQUIRE(fragment_instance.sockets.at(0).is_another_socket_near == true);
    REQUIRE(fragment_instance.sockets.at(0).is_ready_to_connect == true);
    REQUIRE(ji.sockets.at(0).is_another_socket_near == true);
    REQUIRE(ji.sockets.at(0).is_ready_to_connect == true);
  }

  SECTION("most proximal joint wins when two joints are within range") {
    // Build two joint instances manually so we control their IDs (and
    // therefore iteration order: lower ID is visited first by std::map).
    // Fragment socket world: (0, 5).
    // joint_far socket world: (8, 0). Distance ≈ 9.43 — proximity only.
    // joint_near socket world: (0, 4). Distance = 1 — ready to connect.
    JointInstance joint_far = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
    joint_far.sockets.at(0).local_position = {0.f, 0.f};
    joint_far.transform.translate({8.f, 0.f});

    JointInstance joint_near = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
    joint_near.sockets.at(0).local_position = {0.f, 0.f};
    joint_near.transform.translate({0.f, 4.f});

    // joint_far gets the lower ID → visited first
    const uint32_t far_id = joint_far.id;
    const uint32_t near_id = joint_near.id;
    PartGraph part_graph;
    part_graph.emplace(far_id, std::move(joint_far));
    part_graph.emplace(near_id, std::move(joint_near));

    check_socket_collisions(fragment_instance, part_graph);

    // fragment socket must reflect the nearest candidate
    REQUIRE(fragment_instance.sockets.at(0).is_ready_to_connect == true);
    REQUIRE(
        fragment_instance.sockets.at(0).distance_to_nearest_socket.has_value());
    REQUIRE_THAT(
        fragment_instance.sockets.at(0).distance_to_nearest_socket.value(),
        Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // near joint socket must be ready to connect
    const JointInstance &near_ref =
        std::get<JointInstance>(part_graph.at(near_id));
    REQUIRE(near_ref.sockets.at(0).is_ready_to_connect == true);

    // far joint socket must be proximity-only (not ready)
    const JointInstance &far_ref =
        std::get<JointInstance>(part_graph.at(far_id));
    REQUIRE(far_ref.sockets.at(0).is_another_socket_near == true);
    REQUIRE(far_ref.sockets.at(0).is_ready_to_connect == false);
  }

  SECTION("stale state from a previous tick is cleared at the start of each "
          "call") {
    // Arrange
    PartGraph part_graph =
        PartGraphBuilder{}
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f0")
            .AddJointInstance(parts::JointSquareWithOneSocket, "j0")
            .Build()
            .part_graph;
    FragmentInstance &fragment_instance_one =
        std::get<FragmentInstance>(part_graph.at(0));
    JointInstance &joint_instance = std::get<JointInstance>(part_graph.at(1));
    joint_instance.sockets.at(0).local_position = {0.f, 0.f};

    // first tick: sockets close — state is set
    // Fragment socket world: (0, 5). Joint translate (0, 3) → socket (0, 3).
    // Distance = sqrt(0 + (5-3)^2) = 2, within connection threshold (2.5).
    joint_instance.transform.translate({0.f, 3.f});
    check_socket_collisions(fragment_instance_one, part_graph);
    REQUIRE(fragment_instance_one.sockets.at(0).is_ready_to_connect == true);
    REQUIRE(joint_instance.sockets.at(0).is_ready_to_connect == true);

    // second tick: joint moves far away
    joint_instance.transform = sf::Transform::Identity;
    joint_instance.transform.translate({200.f, 0.f});
    check_socket_collisions(fragment_instance_one, part_graph);

    // stale state must have been cleared by the internal reset
    REQUIRE(fragment_instance_one.sockets.at(0).is_another_socket_near ==
            false);
    REQUIRE(fragment_instance_one.sockets.at(0).is_ready_to_connect == false);
    REQUIRE(joint_instance.sockets.at(0).is_another_socket_near == false);
    REQUIRE(joint_instance.sockets.at(0).is_ready_to_connect == false);
  }
}

TEST_CASE(
    "check_socket_collisions(FragmentInstance, PartGraph) multi-socket tests",
    "[unit][collision_grimoire_machina]") {
  PartGraphBuilder builder;

  SECTION("two fragment sockets: each finds its own nearest joint candidate") {
    // FragmentRectangleWithTwoSockets: sockets at local (0,5) and (50,5).
    // Identity transform → world sockets at (0,5) and (50,5).
    //
    // Joint_a socket world: (0, 4). Joint_b socket world: (50, 4).
    //
    // Distances:
    //   F[0](0,5) ↔ J_a(0,4)  = 1   → ready
    //   F[0](0,5) ↔ J_b(50,4) ≈ 50  → far
    //   F[1](50,5) ↔ J_a(0,4) ≈ 50  → far
    //   F[1](50,5) ↔ J_b(50,4) = 1  → ready
    //
    // Each fragment socket pairs exclusively with its nearest joint — two
    // valid ready-to-connect pairs.
    FragmentInstance fi = builder.MakeFragmentInstance(parts::FragmentRectangleWithTwoSockets);
    REQUIRE(fi.sockets.size() == 2);

    JointInstance joint_a = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
    joint_a.sockets.at(0).local_position = {0.f, 0.f};
    joint_a.transform.translate({0.f, 4.f});

    JointInstance joint_b = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
    joint_b.sockets.at(0).local_position = {0.f, 0.f};
    joint_b.transform.translate({50.f, 4.f});

    const uint32_t a_id = joint_a.id;
    const uint32_t b_id = joint_b.id;
    PartGraph part_graph;
    part_graph.emplace(a_id, std::move(joint_a));
    part_graph.emplace(b_id, std::move(joint_b));

    check_socket_collisions(fi, part_graph);

    REQUIRE(fi.sockets.at(0).is_ready_to_connect == true);
    REQUIRE_THAT(fi.sockets.at(0).distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));
    REQUIRE(fi.sockets.at(1).is_ready_to_connect == true);
    REQUIRE_THAT(fi.sockets.at(1).distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));
  }

  SECTION("two fragment sockets: most proximal joint wins per socket") {
    // FragmentRectangleWithTwoSockets: sockets at local (0,5) and (50,5).
    // Both joints clustered near F[0]: J_far at world (8,0), J_near at (0,4).
    //
    // Distances from F[0](0,5):
    //   J_far(8,0)  ≈ 9.43 → near only
    //   J_near(0,4) = 1    → ready
    //
    // F[1](50,5) has no candidate in range.
    FragmentInstance fi = builder.MakeFragmentInstance(parts::FragmentRectangleWithTwoSockets);
    REQUIRE(fi.sockets.size() == 2);

    JointInstance joint_far = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
    joint_far.sockets.at(0).local_position = {0.f, 0.f};
    joint_far.transform.translate({8.f, 0.f});

    JointInstance joint_near = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
    joint_near.sockets.at(0).local_position = {0.f, 0.f};
    joint_near.transform.translate({0.f, 4.f});

    const uint32_t far_id = joint_far.id; // lower ID → visited first
    const uint32_t near_id = joint_near.id;
    PartGraph part_graph;
    part_graph.emplace(far_id, std::move(joint_far));
    part_graph.emplace(near_id, std::move(joint_near));

    check_socket_collisions(fi, part_graph);

    // F[0] records the nearest joint (distance 1, ready)
    REQUIRE(fi.sockets.at(0).is_ready_to_connect == true);
    REQUIRE_THAT(fi.sockets.at(0).distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // F[1] has no candidate in range
    REQUIRE(fi.sockets.at(1).is_another_socket_near == false);
    REQUIRE(fi.sockets.at(1).is_ready_to_connect == false);

    // near joint is ready, far joint is proximity-only
    const JointInstance &near_ref =
        std::get<JointInstance>(part_graph.at(near_id));
    REQUIRE(near_ref.sockets.at(0).is_ready_to_connect == true);

    const JointInstance &far_ref =
        std::get<JointInstance>(part_graph.at(far_id));
    REQUIRE(far_ref.sockets.at(0).is_another_socket_near == true);
    REQUIRE(far_ref.sockets.at(0).is_ready_to_connect == false);
  }

  SECTION("three fragment sockets: only one pair ready to connect") {
    // FragmentRectangleWithThreeSockets: sockets at (0,5), (50,5), (25,0).
    // Three joints placed so only F[0]↔J[0] is within the connection threshold.
    //
    // Joint socket world positions (identity transforms, local pos {0,0}):
    //   J[0]: translate(0, 5) → socket world (0, 5)
    //   J[1]: translate(45, 5) → socket world (45, 5)
    //   J[2]: translate(20, 0) → socket world (20, 0)
    //
    // Key distances (connection threshold = 2.5, proximity threshold = 10):
    //   F[0](0,5) ↔ J[0](0,5)  = 0  → ready to connect (≤ 2.5)
    //   F[1](50,5) ↔ J[1](45,5) = 5 → near only (≤ 10, > 2.5)
    //   F[2](25,0) ↔ J[2](20,0) = 5 → near only (≤ 10, > 2.5)
    FragmentInstance fi{&parts::FragmentRectangleWithThreeSockets};
    REQUIRE(fi.sockets.size() == 3);

    JointInstance joint_0 = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
    joint_0.sockets.at(0).local_position = {0.f, 0.f};
    joint_0.transform.translate({0.f, 5.f});

    JointInstance joint_1 = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
    joint_1.sockets.at(0).local_position = {0.f, 0.f};
    joint_1.transform.translate({45.f, 5.f});

    JointInstance joint_2 = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
    joint_2.sockets.at(0).local_position = {0.f, 0.f};
    joint_2.transform.translate({20.f, 0.f});

    const uint32_t id_0 = joint_0.id;
    const uint32_t id_1 = joint_1.id;
    const uint32_t id_2 = joint_2.id;
    PartGraph part_graph;
    part_graph.emplace(id_0, std::move(joint_0));
    part_graph.emplace(id_1, std::move(joint_1));
    part_graph.emplace(id_2, std::move(joint_2));

    check_socket_collisions(fi, part_graph);

    // count ready sockets on each side — only one pair should be ready
    int fragment_ready = 0;
    for (const auto &[id, socket_data] : fi.sockets)
      if (socket_data.is_ready_to_connect)
        fragment_ready++;

    int joint_ready = 0;
    for (const auto &[id, variant] : part_graph) {
      if (const auto *ji = std::get_if<JointInstance>(&variant))
        if (ji->sockets.at(0).is_ready_to_connect)
          joint_ready++;
    }

    REQUIRE(fragment_ready == 1);
    REQUIRE(joint_ready == 1);

    // the ready pair must be F[0] and J[0]
    REQUIRE(fi.sockets.at(0).is_ready_to_connect == true);
    REQUIRE(fi.sockets.at(1).is_ready_to_connect == false);
    REQUIRE(fi.sockets.at(2).is_ready_to_connect == false);
    REQUIRE(std::get<JointInstance>(part_graph.at(id_0))
                .sockets.at(0)
                .is_ready_to_connect == true);
    REQUIRE(std::get<JointInstance>(part_graph.at(id_1))
                .sockets.at(0)
                .is_ready_to_connect == false);
    REQUIRE(std::get<JointInstance>(part_graph.at(id_2))
                .sockets.at(0)
                .is_ready_to_connect == false);

    // non-ready sockets still reflect proximity state where applicable
    REQUIRE(fi.sockets.at(1).is_another_socket_near == true);
    REQUIRE(fi.sockets.at(2).is_another_socket_near == true);
  }
}

TEST_CASE("check_socket_collisions(JointInstance, PartGraph) tests",
          "[unit][collision_grimoire_machina]") {
  PartGraphBuilder builder;

  // JointSquareWithOneSocket: one socket; local position set to (0, 0).
  // Identity transform → joint socket world (0, 0).
  JointInstance joint_instance = builder.MakeJointInstance(parts::JointSquareWithOneSocket);
  REQUIRE(joint_instance.sockets.size() == 1);
  joint_instance.sockets.at(0).local_position = {0.f, 0.f};

  SECTION("does not throw on empty PartGraph") {
    PartGraph empty_map;
    REQUIRE_NOTHROW(
        check_socket_collisions(joint_instance, empty_map));
    REQUIRE(joint_instance.sockets.at(0).is_another_socket_near == false);
    REQUIRE(joint_instance.sockets.at(0).is_ready_to_connect == false);
  }

  SECTION("no collision when fragment socket is far away") {
    PartGraph part_graph =
        PartGraphBuilder{}
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f0")
            .Build()
            .part_graph;
    FragmentInstance &fi =
        std::get<FragmentInstance>(part_graph.begin()->second);

    // Fragment socket local (0, 5) + translate (100, 0) → world (100, 5).
    // Distance to joint socket (0, 0) ≈ 100 — far.
    fi.transform.translate({100.f, 0.f});

    check_socket_collisions(joint_instance, part_graph);

    REQUIRE(joint_instance.sockets.at(0).is_another_socket_near == false);
    REQUIRE(joint_instance.sockets.at(0).is_ready_to_connect == false);
    REQUIRE(fi.sockets.at(0).is_another_socket_near == false);
    REQUIRE(fi.sockets.at(0).is_ready_to_connect == false);
  }

  SECTION("is_another_socket_near is true when fragment socket is within "
          "proximity") {
    PartGraph part_graph =
        PartGraphBuilder{}
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f0")
            .Build()
            .part_graph;
    FragmentInstance &fi =
        std::get<FragmentInstance>(part_graph.begin()->second);

    // Fragment socket local (0, 5) + translate (5, 0) → world (5, 5).
    // Distance to joint socket (0, 0) = sqrt(25+25) ≈ 7.07, within
    // proximity (10).
    fi.transform.translate({5.f, 0.f});

    check_socket_collisions(joint_instance, part_graph);

    REQUIRE(joint_instance.sockets.at(0).is_another_socket_near == true);
    REQUIRE(joint_instance.sockets.at(0).is_ready_to_connect == false);
    REQUIRE(fi.sockets.at(0).is_another_socket_near == true);
    REQUIRE(fi.sockets.at(0).is_ready_to_connect == false);
  }

  SECTION("is_ready_to_connect is true when fragment socket is within "
          "connection threshold") {
    PartGraph part_graph =
        PartGraphBuilder{}
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f0")
            .Build()
            .part_graph;
    FragmentInstance &fi =
        std::get<FragmentInstance>(part_graph.begin()->second);

    // Fragment socket local (0, 5) + translate (1, -4) → world (1, 1).
    // Distance to joint socket (0, 0) = sqrt(1+1) ≈ 1.41, within
    // connection threshold (2.5).
    fi.transform.translate({1.f, -4.f});

    check_socket_collisions(joint_instance, part_graph);

    REQUIRE(joint_instance.sockets.at(0).is_another_socket_near == true);
    REQUIRE(joint_instance.sockets.at(0).is_ready_to_connect == true);
    REQUIRE(fi.sockets.at(0).is_another_socket_near == true);
    REQUIRE(fi.sockets.at(0).is_ready_to_connect == true);
  }

  SECTION("most proximal fragment wins when two fragments are within range") {
    // Fragment socket local (0, 5).
    // frag_far:  translate (8, 0) → socket world (8, 5). Distance ≈ 9.43 —
    //            proximity only.
    // frag_near: translate (0, -4) → socket world (0, 1). Distance = 1 —
    //            ready to connect.
    FragmentInstance frag_far =
        builder.MakeFragmentInstance(parts::FragmentRectangleWithOneSocket);
    frag_far.transform.translate({8.f, 0.f});

    FragmentInstance frag_near =
        builder.MakeFragmentInstance(parts::FragmentRectangleWithOneSocket);
    frag_near.transform.translate({0.f, -4.f});

    const uint32_t far_id = frag_far.id; // lower ID → visited first
    const uint32_t near_id = frag_near.id;
    PartGraph part_graph;
    part_graph.emplace(far_id, std::move(frag_far));
    part_graph.emplace(near_id, std::move(frag_near));

    check_socket_collisions(joint_instance, part_graph);

    // joint socket must reflect the nearest candidate
    REQUIRE(joint_instance.sockets.at(0).is_ready_to_connect == true);
    REQUIRE(
        joint_instance.sockets.at(0).distance_to_nearest_socket.has_value());
    REQUIRE_THAT(
        joint_instance.sockets.at(0).distance_to_nearest_socket.value(),
        Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // near fragment socket must be ready to connect
    const FragmentInstance &near_ref =
        std::get<FragmentInstance>(part_graph.at(near_id));
    REQUIRE(near_ref.sockets.at(0).is_ready_to_connect == true);

    // far fragment socket must be proximity-only (not ready)
    const FragmentInstance &far_ref =
        std::get<FragmentInstance>(part_graph.at(far_id));
    REQUIRE(far_ref.sockets.at(0).is_another_socket_near == true);
    REQUIRE(far_ref.sockets.at(0).is_ready_to_connect == false);
  }

  SECTION("stale state from a previous tick is cleared at the start of each "
          "call") {
    PartGraph part_graph =
        PartGraphBuilder{}
            .AddFragmentInstance(parts::FragmentRectangleWithOneSocket, "f0")
            .Build()
            .part_graph;
    FragmentInstance &fi =
        std::get<FragmentInstance>(part_graph.begin()->second);

    // first tick: sockets close — state is set
    // Fragment socket local (0,5) + translate (1,-4) → world (1,1).
    // Distance to joint (0,0) ≈ 1.41, within connection threshold (2.5).
    fi.transform.translate({1.f, -4.f});
    check_socket_collisions(joint_instance, part_graph);
    REQUIRE(joint_instance.sockets.at(0).is_ready_to_connect == true);
    REQUIRE(fi.sockets.at(0).is_ready_to_connect == true);

    // second tick: fragment moves far away
    fi.transform = sf::Transform::Identity;
    fi.transform.translate({100.f, 0.f});
    check_socket_collisions(joint_instance, part_graph);

    // stale state must have been cleared by the internal reset
    REQUIRE(joint_instance.sockets.at(0).is_another_socket_near == false);
    REQUIRE(joint_instance.sockets.at(0).is_ready_to_connect == false);
    REQUIRE(fi.sockets.at(0).is_another_socket_near == false);
    REQUIRE(fi.sockets.at(0).is_ready_to_connect == false);
  }
}

TEST_CASE(
    "check_socket_collisions(JointInstance, PartGraph) multi-socket tests",
    "[unit][collision_grimoire_machina]") {
  PartGraphBuilder builder;

  SECTION("two joint sockets: each finds its own nearest fragment candidate") {
    // Joint uses identity transform; sockets at local/world (0,0) and (20,0).
    // Fragment socket local (0,5):
    //   frag_a translate (0,-4) → world (0,1). J[0] distance = 1 → ready.
    //   frag_b translate (20,-4) → world (20,1). J[1] distance = 1 → ready.
    JointInstance ji = builder.MakeJointInstance(parts::JointSquareWith2Sockets);
    REQUIRE(ji.sockets.size() == 2);
    ji.sockets.at(0).local_position = {0.f, 0.f};
    ji.sockets.at(1).local_position = {20.f, 0.f};

    FragmentInstance frag_a = builder.MakeFragmentInstance(parts::FragmentRectangleWithOneSocket);
    frag_a.transform.translate({0.f, -4.f});

    FragmentInstance frag_b = builder.MakeFragmentInstance(parts::FragmentRectangleWithOneSocket);
    frag_b.transform.translate({20.f, -4.f});

    const uint32_t a_id = frag_a.id;
    const uint32_t b_id = frag_b.id;
    PartGraph part_graph;
    part_graph.emplace(a_id, std::move(frag_a));
    part_graph.emplace(b_id, std::move(frag_b));

    check_socket_collisions(ji, part_graph);

    REQUIRE(ji.sockets.at(0).is_ready_to_connect == true);
    REQUIRE_THAT(ji.sockets.at(0).distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));
    REQUIRE(ji.sockets.at(1).is_ready_to_connect == true);
    REQUIRE_THAT(ji.sockets.at(1).distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));
  }

  SECTION("two joint sockets: most proximal fragment wins per socket") {
    // Joint sockets at world (0,0) and (20,0) — far apart.
    // Fragment socket local (0,5). Both fragments near x=0:
    //   frag_far:  translate (8, 0) → socket world (8, 5). Distance ≈ 9.43
    //              from J[0] — proximity only.
    //   frag_near: translate (0,-4) → socket world (0, 1). Distance = 1
    //              from J[0] — ready to connect.
    // J[1](20,0) has no candidate in range.
    JointInstance ji = builder.MakeJointInstance(parts::JointSquareWith2Sockets);
    REQUIRE(ji.sockets.size() == 2);
    ji.sockets.at(0).local_position = {0.f, 0.f};
    ji.sockets.at(1).local_position = {20.f, 0.f};

    FragmentInstance frag_far = builder.MakeFragmentInstance(parts::FragmentRectangleWithOneSocket);
    frag_far.transform.translate({8.f, 0.f});

    FragmentInstance frag_near = builder.MakeFragmentInstance(parts::FragmentRectangleWithOneSocket);
    frag_near.transform.translate({0.f, -4.f});

    const uint32_t far_id = frag_far.id;
    const uint32_t near_id = frag_near.id;
    PartGraph part_graph;
    part_graph.emplace(far_id, std::move(frag_far));
    part_graph.emplace(near_id, std::move(frag_near));

    check_socket_collisions(ji, part_graph);

    // J[0] records nearest fragment (distance 1, ready)
    REQUIRE(ji.sockets.at(0).is_ready_to_connect == true);
    REQUIRE_THAT(ji.sockets.at(0).distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // J[1] has no candidate in range
    REQUIRE(ji.sockets.at(1).is_another_socket_near == false);
    REQUIRE(ji.sockets.at(1).is_ready_to_connect == false);
  }

  SECTION("three joint sockets: only one pair ready to connect") {
    // Joint sockets at world (0,0), (10,0), (20,0).
    // Fragment socket local (0,5):
    //   frag_0: translate (0,-4) → world (0, 1). J[0] distance = 1 → ready.
    //   frag_1: translate (15, 0) → world (15, 5). J[1] distance ≈ 7.07 → near.
    //   frag_2: translate (25, 0) → world (25, 5). J[2] distance ≈ 7.07 → near.
    JointInstance ji = builder.MakeJointInstance(parts::JointSquareWithThreeSockets);
    REQUIRE(ji.sockets.size() == 3);
    ji.sockets.at(0).local_position = {0.f, 0.f};
    ji.sockets.at(1).local_position = {10.f, 0.f};
    ji.sockets.at(2).local_position = {20.f, 0.f};

    FragmentInstance frag_0 = builder.MakeFragmentInstance(parts::FragmentRectangleWithOneSocket);
    frag_0.transform.translate({0.f, -4.f});

    FragmentInstance frag_1 = builder.MakeFragmentInstance(parts::FragmentRectangleWithOneSocket);
    frag_1.transform.translate({15.f, 0.f});

    FragmentInstance frag_2 = builder.MakeFragmentInstance(parts::FragmentRectangleWithOneSocket);
    frag_2.transform.translate({25.f, 0.f});

    const uint32_t id_0 = frag_0.id;
    const uint32_t id_1 = frag_1.id;
    const uint32_t id_2 = frag_2.id;
    PartGraph part_graph;
    part_graph.emplace(id_0, std::move(frag_0));
    part_graph.emplace(id_1, std::move(frag_1));
    part_graph.emplace(id_2, std::move(frag_2));

    check_socket_collisions(ji, part_graph);

    // only one joint socket should be ready to connect
    int joint_ready = 0;
    for (const auto &[id, socket_data] : ji.sockets)
      if (socket_data.is_ready_to_connect)
        joint_ready++;

    int frag_ready = 0;
    for (const auto &[id, variant] : part_graph) {
      if (const auto *fi = std::get_if<FragmentInstance>(&variant))
        if (fi->sockets.at(0).is_ready_to_connect)
          frag_ready++;
    }

    REQUIRE(joint_ready == 1);
    REQUIRE(frag_ready == 1);

    // specifically J[0] and frag_0
    REQUIRE(ji.sockets.at(0).is_ready_to_connect == true);
    REQUIRE(ji.sockets.at(1).is_ready_to_connect == false);
    REQUIRE(ji.sockets.at(2).is_ready_to_connect == false);
    REQUIRE(std::get<FragmentInstance>(part_graph.at(id_0))
                .sockets.at(0)
                .is_ready_to_connect == true);
    REQUIRE(std::get<FragmentInstance>(part_graph.at(id_1))
                .sockets.at(0)
                .is_ready_to_connect == false);
    REQUIRE(std::get<FragmentInstance>(part_graph.at(id_2))
                .sockets.at(0)
                .is_ready_to_connect == false);

    // non-ready joint sockets still reflect proximity state where applicable
    REQUIRE(ji.sockets.at(1).is_another_socket_near == true);
    REQUIRE(ji.sockets.at(2).is_another_socket_near == true);
  }
}
} // namespace steamrot::tests
