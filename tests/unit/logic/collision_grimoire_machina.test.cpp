/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for collision checking of GrimoireMachina elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include "Vector2fEqualsMatcher.h"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Helpers
/////////////////////////////////////////////////
namespace {

/// @brief Manually reset proximity state on a SocketData.
///        Simulates what reset_socket_proximity_state() does at PartMap level.
void reset_socket(steamrot::SocketData &s) {
  s.is_another_socket_near = false;
  s.is_ready_to_connect = false;
  s.distance_to_nearest_socket = std::nullopt;
}

} // namespace

TEST_CASE("check_socket_collisions(SocketData, SocketData) tests",
          "[unit][collision_grimoire_machina]") {

  // arrange
  steamrot::SocketData socket_one_data{sf::Vector2f{0.f, 0.f}};
  steamrot::SocketData socket_two_data{sf::Vector2f{0.f, 0.f}};

  SECTION("sockets do not collide when far apart") {
    // arrange
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({100.f, 100.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        socket_one_data, socket_one_transform, socket_two_data,
        socket_two_transform);
    // assert
    REQUIRE(socket_one_data.state == steamrot::SocketState::Available);
    REQUIRE(socket_one_data.is_mouse_over == false);
    REQUIRE(socket_one_data.is_another_socket_near == false);
    REQUIRE(socket_one_data.is_ready_to_connect == false);
    REQUIRE(socket_one_data.local_position == sf::Vector2f{0.f, 0.f});
    REQUIRE(socket_one_data.distance_to_nearest_socket == std::nullopt);

    REQUIRE(socket_two_data.state == steamrot::SocketState::Available);
    REQUIRE(socket_two_data.is_mouse_over == false);
    REQUIRE(socket_two_data.is_another_socket_near == false);
    REQUIRE(socket_two_data.is_ready_to_connect == false);
    REQUIRE_THAT(socket_two_data.local_position,
                 steamrot::tests::EqualsVector2f({0.f, 0.f}));
    REQUIRE(socket_two_data.distance_to_nearest_socket == std::nullopt);
  }

  SECTION("is_another_socket_near is true when sockets are within proximity "
          "threshold") {
    // arrange
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({5.f, 5.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        socket_one_data, socket_one_transform, socket_two_data,
        socket_two_transform);
    // assert
    REQUIRE(socket_one_data.is_another_socket_near == true);
    REQUIRE(socket_one_data.is_ready_to_connect == false);
    REQUIRE(socket_one_data.distance_to_nearest_socket.has_value());
    REQUIRE_THAT(socket_one_data.distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(7.07f, 0.01f));

    REQUIRE(socket_two_data.is_ready_to_connect == false);
    REQUIRE(socket_two_data.is_another_socket_near == true);
    REQUIRE(socket_two_data.distance_to_nearest_socket.has_value());
  }

  SECTION("is_another_socket_near is false when sockets are outside proximity "
          "threshold after being near") {
    // arrange — first tick: sockets near
    sf::Transform socket_one_transform = sf::Transform::Identity;
    socket_one_transform.translate({0.f, 0.f});
    sf::Transform socket_two_transform = sf::Transform::Identity;
    socket_two_transform.translate({5.f, 5.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        socket_one_data, socket_one_transform, socket_two_data,
        socket_two_transform);
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

    // reset state (at PartMap level this is handled by
    // reset_socket_proximity_state)
    reset_socket(socket_one_data);
    reset_socket(socket_two_data);

    // arrange — second tick: sockets far apart
    socket_one_transform.translate({-100.f, -100.f});
    socket_two_transform.translate({100.f, 100.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        socket_one_data, socket_one_transform, socket_two_data,
        socket_two_transform);
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
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        socket_one_data, socket_one_transform, socket_two_data,
        socket_two_transform);
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
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        socket_one_data, socket_one_transform, socket_two_data,
        socket_two_transform);
    // assert
    REQUIRE(socket_one_data.is_another_socket_near == true);
    REQUIRE(socket_one_data.is_ready_to_connect == true);
    REQUIRE(socket_two_data.is_another_socket_near == true);
    REQUIRE(socket_two_data.is_ready_to_connect == true);

    // reset state (at PartMap level this is handled by
    // reset_socket_proximity_state)
    reset_socket(socket_one_data);
    reset_socket(socket_two_data);

    // arrange — second tick: sockets far apart
    socket_one_transform.translate({-100.f, -100.f});
    socket_two_transform.translate({100.f, 100.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        socket_one_data, socket_one_transform, socket_two_data,
        socket_two_transform);
    // assert
    REQUIRE(socket_one_data.is_another_socket_near == false);
    REQUIRE(socket_one_data.is_ready_to_connect == false);
    REQUIRE(socket_two_data.is_another_socket_near == false);
    REQUIRE(socket_two_data.is_ready_to_connect == false);
  }
}

TEST_CASE("check_socket_collisions(FragmentInstance,JointInstance) tests with "
          "single socket collisions",
          "[unit][collision_grimoire_machina]") {

  // arrange
  steamrot::tests::TestPartLibrary part_library =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder(part_library);

  steamrot::FragmentInstance fragment_instance =
      builder.MakeFragmentInstance("fragment_one_socket");
  REQUIRE(fragment_instance.sockets.size() == 1);
  steamrot::SocketData &fragment_socket_data = fragment_instance.sockets[0];
  fragment_socket_data.local_position = {0.f, 0.f}; // set for easy testing

  steamrot::JointInstance joint_instance =
      builder.MakeJointInstance("joint_one_socket");
  REQUIRE(joint_instance.sockets.size() == 1);
  steamrot::SocketData &joint_socket_data = joint_instance.sockets[0];
  joint_socket_data.local_position = {0.f, 0.f}; // set for easy testing

  SECTION("sockets do not collide when far apart") {
    // arrange
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({100.f, 100.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);
    // assert
    steamrot::SocketData &socket_data = fragment_instance.sockets[0];
    REQUIRE(socket_data.state == fragment_socket_data.state);
    REQUIRE(socket_data.is_mouse_over == false);
    REQUIRE(socket_data.is_another_socket_near == false);
    REQUIRE(socket_data.is_ready_to_connect == false);

    REQUIRE(joint_socket_data.state == joint_socket_data.state);
    REQUIRE(joint_socket_data.is_mouse_over == false);
    REQUIRE(joint_socket_data.is_another_socket_near == false);
    REQUIRE(joint_socket_data.is_ready_to_connect == false);
  }

  SECTION("is_another_socket_near is true when sockets are within proximity "
          "threshold") {
    // arrange
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({5.f, 5.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);
    // assert
    steamrot::SocketData &socket_data = fragment_instance.sockets[0];
    REQUIRE(socket_data.is_another_socket_near == true);
    REQUIRE(socket_data.is_ready_to_connect == false);
    REQUIRE(joint_socket_data.is_another_socket_near == true);
    REQUIRE(joint_socket_data.is_ready_to_connect == false);
  }

  SECTION("is_another_socket_near is false when sockets were within proximity "
          "threshold but then moved apart") {
    // arrange — first tick: sockets near
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({5.f, 5.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);
    // assert
    steamrot::SocketData &socket_data = fragment_instance.sockets[0];
    REQUIRE(socket_data.is_another_socket_near == true);
    REQUIRE(socket_data.is_ready_to_connect == false);
    REQUIRE(joint_socket_data.is_another_socket_near == true);
    REQUIRE(joint_socket_data.is_ready_to_connect == false);

    // reset state between ticks
    reset_socket(socket_data);
    reset_socket(joint_socket_data);

    // arrange — second tick: sockets far apart
    fragment_instance.transform.translate({-100.f, -100.f});
    joint_instance.transform.translate({100.f, 100.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);
    // assert
    REQUIRE(socket_data.is_another_socket_near == false);
    REQUIRE(socket_data.is_ready_to_connect == false);
    REQUIRE(joint_socket_data.is_another_socket_near == false);
    REQUIRE(joint_socket_data.is_ready_to_connect == false);
  }

  SECTION(
      "is_ready_to_connect is true when they are within connection threshold") {
    // arrange
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({1.f, 1.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);
    // assert
    steamrot::SocketData &socket_data = fragment_instance.sockets[0];
    REQUIRE(socket_data.is_another_socket_near == true);
    REQUIRE(socket_data.is_ready_to_connect == true);
    REQUIRE(joint_socket_data.is_another_socket_near == true);
    REQUIRE(joint_socket_data.is_ready_to_connect == true);
  }

  SECTION("is_ready_to_connect is false when sockets were within connection "
          "threshold but then moved apart") {
    // arrange — first tick: sockets close enough to connect
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({1.f, 1.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);
    // assert
    steamrot::SocketData &socket_data = fragment_instance.sockets[0];
    REQUIRE(socket_data.is_another_socket_near == true);
    REQUIRE(socket_data.is_ready_to_connect == true);
    REQUIRE(joint_socket_data.is_another_socket_near == true);
    REQUIRE(joint_socket_data.is_ready_to_connect == true);

    // reset state between ticks
    reset_socket(socket_data);
    reset_socket(joint_socket_data);

    // arrange — second tick: sockets far apart
    fragment_instance.transform.translate({-100.f, -100.f});
    joint_instance.transform.translate({100.f, 100.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);
    // assert
    REQUIRE(socket_data.is_another_socket_near == false);
    REQUIRE(socket_data.is_ready_to_connect == false);
    REQUIRE(joint_socket_data.is_another_socket_near == false);
    REQUIRE(joint_socket_data.is_ready_to_connect == false);
  }
}

TEST_CASE("check_socket_collisions(FragmentInstance, JointInstance) tests with "
          "multiple socket collisions",
          "[unit][collision_grimoire_machina]") {
  // arrange
  steamrot::tests::TestPartLibrary part_library =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder(part_library);

  steamrot::FragmentInstance fragment_instance =
      builder.MakeFragmentInstance("fragment_two_sockets");
  REQUIRE(fragment_instance.sockets.size() == 2);
  steamrot::SocketData &fragment_socket_data_one = fragment_instance.sockets[0];
  steamrot::SocketData &fragment_socket_data_two = fragment_instance.sockets[1];
  fragment_socket_data_one.local_position = {0.f, 0.f};  // set for easy testing
  fragment_socket_data_two.local_position = {10.f, 0.f}; // set for easy testing

  steamrot::JointInstance joint_instance =
      builder.MakeJointInstance("joint_two_sockets");
  REQUIRE(joint_instance.sockets.size() == 2);
  steamrot::SocketData &joint_socket_data_one = joint_instance.sockets[0];
  steamrot::SocketData &joint_socket_data_two = joint_instance.sockets[1];
  joint_socket_data_one.local_position = {0.f, 0.f};  // set for easy testing
  joint_socket_data_two.local_position = {10.f, 0.f}; // set for easy testing

  SECTION("sockets do not collide when far apart") {
    // arrange
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({100.f, 100.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);
    // assert
    for (steamrot::SocketData &socket_data : fragment_instance.sockets) {
      REQUIRE(socket_data.state == steamrot::SocketState::Available);
      REQUIRE(socket_data.is_mouse_over == false);
      REQUIRE(socket_data.is_another_socket_near == false);
      REQUIRE(socket_data.is_ready_to_connect == false);
    }
    for (steamrot::SocketData &socket_data : joint_instance.sockets) {
      REQUIRE(socket_data.state == steamrot::SocketState::Available);
      REQUIRE(socket_data.is_mouse_over == false);
      REQUIRE(socket_data.is_another_socket_near == false);
      REQUIRE(socket_data.is_ready_to_connect == false);
    }
  }

  SECTION("two sockets: most proximal joint socket wins per fragment socket") {
    // Arrange geometry:
    //   Fragment sockets at world (0,0) and (20,0) — far enough apart that
    //   each can identify its own closest partner without interference.
    //   Joint sockets at world (1,0) and (8,0).
    //
    // Distances (identity transforms, local pos overrides applied above):
    //   F[0](0,0) ↔ J[0](1,0)  = 1   → ready to connect
    //   F[0](0,0) ↔ J[1](8,0)  = 8   → near only
    //   F[1](20,0) ↔ J[0](1,0) = 19  → far (no update)
    //   F[1](20,0) ↔ J[1](8,0) = 12  → far (no update)
    //
    // Expected: F[0] records J[0] (distance 1, ready) ignoring the farther
    // J[1]; J[0] is ready; J[1] is near (distance 8, from F[0]); F[1] has
    // no collision.
    fragment_socket_data_one.local_position = {0.f, 0.f};
    fragment_socket_data_two.local_position = {20.f, 0.f};
    joint_socket_data_one.local_position = {1.f, 0.f};
    joint_socket_data_two.local_position = {8.f, 0.f};

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);

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
  steamrot::tests::TestPartLibrary part_library =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder(part_library);

  steamrot::FragmentInstance fragment_instance =
      builder.MakeFragmentInstance("fragment_three_sockets");
  REQUIRE(fragment_instance.sockets.size() == 3);

  steamrot::JointInstance joint_instance =
      builder.MakeJointInstance("joint_three_sockets");
  REQUIRE(joint_instance.sockets.size() == 3);

  // Override socket positions to known values for predictable geometry.
  // Fragment sockets at (0,0), (10,0), (20,0).
  // Joint  sockets at (1,0), (15,0), (25,0).
  //
  // Pair distances (identity transforms):
  //   F[0](0,0)  ↔ J[0](1,0)   = 1   → ready to connect
  //   F[0](0,0)  ↔ J[1](15,0)  = 15  → far
  //   F[0](0,0)  ↔ J[2](25,0)  = 25  → far
  //   F[1](10,0) ↔ J[0](1,0)   = 9   → near
  //   F[1](10,0) ↔ J[1](15,0)  = 5   → near (best for F[1])
  //   F[1](10,0) ↔ J[2](25,0)  = 15  → far
  //   F[2](20,0) ↔ J[0](1,0)   = 19  → far
  //   F[2](20,0) ↔ J[1](15,0)  = 5   → near
  //   F[2](20,0) ↔ J[2](25,0)  = 5   → near (tie; J[1] visited first)
  //
  // Expected: only the F[0]–J[0] pair is ready to connect.
  fragment_instance.sockets[0].local_position = {0.f, 0.f};
  fragment_instance.sockets[1].local_position = {10.f, 0.f};
  fragment_instance.sockets[2].local_position = {20.f, 0.f};
  joint_instance.sockets[0].local_position = {1.f, 0.f};
  joint_instance.sockets[1].local_position = {15.f, 0.f};
  joint_instance.sockets[2].local_position = {25.f, 0.f};

  SECTION("three sockets: only the nearest pair is ready to connect") {
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);

    // only F[0] ↔ J[0] pair should be ready to connect
    int fragment_ready_count = 0;
    for (const steamrot::SocketData &s : fragment_instance.sockets)
      if (s.is_ready_to_connect)
        fragment_ready_count++;

    int joint_ready_count = 0;
    for (const steamrot::SocketData &s : joint_instance.sockets)
      if (s.is_ready_to_connect)
        joint_ready_count++;

    REQUIRE(fragment_ready_count == 1);
    REQUIRE(joint_ready_count == 1);

    // verify it is specifically F[0] and J[0]
    REQUIRE(fragment_instance.sockets[0].is_ready_to_connect == true);
    REQUIRE(fragment_instance.sockets[1].is_ready_to_connect == false);
    REQUIRE(fragment_instance.sockets[2].is_ready_to_connect == false);
    REQUIRE(joint_instance.sockets[0].is_ready_to_connect == true);
    REQUIRE(joint_instance.sockets[1].is_ready_to_connect == false);
    REQUIRE(joint_instance.sockets[2].is_ready_to_connect == false);

    // non-ready sockets in proximity should still reflect their best candidate
    REQUIRE(fragment_instance.sockets[1].is_another_socket_near == true);
    REQUIRE(fragment_instance.sockets[2].is_another_socket_near == true);
    REQUIRE(joint_instance.sockets[1].is_another_socket_near == true);
  }
}

TEST_CASE("reset_socket_proximity_state(PartMap) tests",
          "[unit][collision_grimoire_machina]") {
  steamrot::tests::TestPartLibrary part_library =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder(part_library);

  SECTION("does not throw on an empty PartMap") {
    steamrot::PartMap empty_map;
    REQUIRE_NOTHROW(steamrot::logic::collision::grimoire_machina::
                        reset_socket_proximity_state(empty_map));
  }

  SECTION("clears proximity state on all sockets in the PartMap") {
    steamrot::PartMap part_map =
        builder.MakePartMap({"fragment_one_socket"}, {"joint_one_socket"});

    // manually set proximity state on every socket
    for (auto &[id, variant] : part_map) {
      auto visit = [](auto &instance) {
        for (steamrot::SocketData &s : instance.sockets) {
          s.is_another_socket_near = true;
          s.is_ready_to_connect = true;
          s.distance_to_nearest_socket = 3.f;
        }
      };
      if (auto *fi = std::get_if<steamrot::FragmentInstance>(&variant))
        visit(*fi);
      else if (auto *ji = std::get_if<steamrot::JointInstance>(&variant))
        visit(*ji);
    }

    // act
    steamrot::logic::collision::grimoire_machina::reset_socket_proximity_state(
        part_map);

    // assert — all sockets should be back to default
    for (auto &[id, variant] : part_map) {
      auto check = [](const auto &instance) {
        for (const steamrot::SocketData &s : instance.sockets) {
          REQUIRE(s.is_another_socket_near == false);
          REQUIRE(s.is_ready_to_connect == false);
          REQUIRE(s.distance_to_nearest_socket == std::nullopt);
        }
      };
      if (const auto *fi = std::get_if<steamrot::FragmentInstance>(&variant))
        check(*fi);
      else if (const auto *ji = std::get_if<steamrot::JointInstance>(&variant))
        check(*ji);
    }
  }
}

TEST_CASE("check_socket_collisions(FragmentInstance, PartMap) tests",
          "[unit][collision_grimoire_machina]") {
  steamrot::tests::TestPartLibrary part_library =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder(part_library);

  // Fragment with one socket at local (0,0); identity transform → world (0,0)
  steamrot::FragmentInstance fragment_instance =
      builder.MakeFragmentInstance("fragment_one_socket");
  REQUIRE(fragment_instance.sockets.size() == 1);
  fragment_instance.sockets[0].local_position = {0.f, 0.f};

  SECTION("does not throw on empty PartMap") {
    steamrot::PartMap empty_map;
    REQUIRE_NOTHROW(
        steamrot::logic::collision::grimoire_machina::check_socket_collisions(
            fragment_instance, empty_map));
    REQUIRE(fragment_instance.sockets[0].is_another_socket_near == false);
    REQUIRE(fragment_instance.sockets[0].is_ready_to_connect == false);
  }

  SECTION("no collision when joint socket is far away") {
    steamrot::PartMap part_map = builder.MakePartMap({}, {"joint_one_socket"});
    steamrot::JointInstance &ji =
        std::get<steamrot::JointInstance>(part_map.begin()->second);
    ji.sockets[0].local_position = {0.f, 0.f};
    ji.transform.translate({100.f, 0.f}); // world pos (100,0) — distance 100

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, part_map);

    REQUIRE(fragment_instance.sockets[0].is_another_socket_near == false);
    REQUIRE(fragment_instance.sockets[0].is_ready_to_connect == false);
    REQUIRE(ji.sockets[0].is_another_socket_near == false);
    REQUIRE(ji.sockets[0].is_ready_to_connect == false);
  }

  SECTION(
      "is_another_socket_near is true when joint socket is within proximity") {
    steamrot::PartMap part_map = builder.MakePartMap({}, {"joint_one_socket"});
    steamrot::JointInstance &ji =
        std::get<steamrot::JointInstance>(part_map.begin()->second);
    ji.sockets[0].local_position = {0.f, 0.f};
    ji.transform.translate(
        {5.f, 5.f}); // distance ≈ 7.07, within proximity (10)

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, part_map);

    REQUIRE(fragment_instance.sockets[0].is_another_socket_near == true);
    REQUIRE(fragment_instance.sockets[0].is_ready_to_connect == false);
    REQUIRE(ji.sockets[0].is_another_socket_near == true);
    REQUIRE(ji.sockets[0].is_ready_to_connect == false);
  }

  SECTION("is_ready_to_connect is true when joint socket is within connection "
          "threshold") {
    steamrot::PartMap part_map = builder.MakePartMap({}, {"joint_one_socket"});
    steamrot::JointInstance &ji =
        std::get<steamrot::JointInstance>(part_map.begin()->second);
    ji.sockets[0].local_position = {0.f, 0.f};
    ji.transform.translate(
        {1.f, 1.f}); // distance ≈ 1.41, within connection (2.5)

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, part_map);

    REQUIRE(fragment_instance.sockets[0].is_another_socket_near == true);
    REQUIRE(fragment_instance.sockets[0].is_ready_to_connect == true);
    REQUIRE(ji.sockets[0].is_another_socket_near == true);
    REQUIRE(ji.sockets[0].is_ready_to_connect == true);
  }

  SECTION("most proximal joint wins when two joints are within range") {
    // Build two joint instances manually so we control their IDs (and
    // therefore iteration order: lower ID is visited first by std::map).
    steamrot::JointInstance joint_far =
        builder.MakeJointInstance("joint_one_socket");
    joint_far.sockets[0].local_position = {0.f, 0.f};
    joint_far.transform.translate({8.f, 0.f}); // distance 8 — proximity only

    steamrot::JointInstance joint_near =
        builder.MakeJointInstance("joint_one_socket");
    joint_near.sockets[0].local_position = {0.f, 0.f};
    joint_near.transform.translate({1.f, 0.f}); // distance 1 — ready to connect

    // joint_far gets the lower ID → visited first
    const uint32_t far_id = joint_far.id;
    const uint32_t near_id = joint_near.id;
    steamrot::PartMap part_map;
    part_map.emplace(far_id, std::move(joint_far));
    part_map.emplace(near_id, std::move(joint_near));

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, part_map);

    // fragment socket must reflect the nearest candidate
    REQUIRE(fragment_instance.sockets[0].is_ready_to_connect == true);
    REQUIRE(
        fragment_instance.sockets[0].distance_to_nearest_socket.has_value());
    REQUIRE_THAT(
        fragment_instance.sockets[0].distance_to_nearest_socket.value(),
        Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // near joint socket must be ready to connect
    const steamrot::JointInstance &near_ref =
        std::get<steamrot::JointInstance>(part_map.at(near_id));
    REQUIRE(near_ref.sockets[0].is_ready_to_connect == true);

    // far joint socket must be proximity-only (not ready)
    const steamrot::JointInstance &far_ref =
        std::get<steamrot::JointInstance>(part_map.at(far_id));
    REQUIRE(far_ref.sockets[0].is_another_socket_near == true);
    REQUIRE(far_ref.sockets[0].is_ready_to_connect == false);
  }

  SECTION("stale state from a previous tick is cleared at the start of each "
          "call") {
    steamrot::PartMap part_map = builder.MakePartMap({}, {"joint_one_socket"});
    steamrot::JointInstance &ji =
        std::get<steamrot::JointInstance>(part_map.begin()->second);
    ji.sockets[0].local_position = {0.f, 0.f};

    // first tick: sockets close — state is set
    ji.transform.translate({1.f, 1.f});
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, part_map);
    REQUIRE(fragment_instance.sockets[0].is_ready_to_connect == true);
    REQUIRE(ji.sockets[0].is_ready_to_connect == true);

    // second tick: joint moves far away
    ji.transform = sf::Transform::Identity;
    ji.transform.translate({100.f, 0.f});
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, part_map);

    // stale state must have been cleared by the internal reset
    REQUIRE(fragment_instance.sockets[0].is_another_socket_near == false);
    REQUIRE(fragment_instance.sockets[0].is_ready_to_connect == false);
    REQUIRE(ji.sockets[0].is_another_socket_near == false);
    REQUIRE(ji.sockets[0].is_ready_to_connect == false);
  }
}

TEST_CASE("check_socket_collisions(FragmentInstance, PartMap) multi-socket tests",
          "[unit][collision_grimoire_machina]") {
  steamrot::tests::TestPartLibrary part_library =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder(part_library);

  SECTION("two fragment sockets: each finds its own nearest joint candidate") {
    // Fragment sockets at world (0,0) and (20,0).
    // Two joints in PartMap with sockets at world (1,0) and (19,0).
    //
    // Distances:
    //   F[0](0,0)  ↔ J_a[0](1,0)  = 1   → ready
    //   F[0](0,0)  ↔ J_b[0](19,0) = 19  → far
    //   F[1](20,0) ↔ J_a[0](1,0)  = 19  → far
    //   F[1](20,0) ↔ J_b[0](19,0) = 1   → ready
    //
    // Each fragment socket pairs exclusively with its nearest joint — two
    // valid ready-to-connect pairs.
    steamrot::FragmentInstance fi =
        builder.MakeFragmentInstance("fragment_two_sockets");
    REQUIRE(fi.sockets.size() == 2);
    fi.sockets[0].local_position = {0.f, 0.f};
    fi.sockets[1].local_position = {20.f, 0.f};

    steamrot::JointInstance joint_a =
        builder.MakeJointInstance("joint_one_socket");
    joint_a.sockets[0].local_position = {0.f, 0.f};
    joint_a.transform.translate({1.f, 0.f});

    steamrot::JointInstance joint_b =
        builder.MakeJointInstance("joint_one_socket");
    joint_b.sockets[0].local_position = {0.f, 0.f};
    joint_b.transform.translate({19.f, 0.f});

    const uint32_t a_id = joint_a.id;
    const uint32_t b_id = joint_b.id;
    steamrot::PartMap part_map;
    part_map.emplace(a_id, std::move(joint_a));
    part_map.emplace(b_id, std::move(joint_b));

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fi, part_map);

    REQUIRE(fi.sockets[0].is_ready_to_connect == true);
    REQUIRE_THAT(fi.sockets[0].distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));
    REQUIRE(fi.sockets[1].is_ready_to_connect == true);
    REQUIRE_THAT(fi.sockets[1].distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));
  }

  SECTION("two fragment sockets: most proximal joint wins per socket") {
    // Fragment sockets at world (0,0) and (20,0) — far apart.
    // Both joints clustered near x=0: J_far at x=8 (near), J_near at x=1 (ready).
    // F[0] should prefer J_near; F[1] has no candidate in range.
    steamrot::FragmentInstance fi =
        builder.MakeFragmentInstance("fragment_two_sockets");
    REQUIRE(fi.sockets.size() == 2);
    fi.sockets[0].local_position = {0.f, 0.f};
    fi.sockets[1].local_position = {20.f, 0.f};

    steamrot::JointInstance joint_far =
        builder.MakeJointInstance("joint_one_socket");
    joint_far.sockets[0].local_position = {0.f, 0.f};
    joint_far.transform.translate({8.f, 0.f}); // distance 8 from F[0]

    steamrot::JointInstance joint_near =
        builder.MakeJointInstance("joint_one_socket");
    joint_near.sockets[0].local_position = {0.f, 0.f};
    joint_near.transform.translate({1.f, 0.f}); // distance 1 from F[0]

    const uint32_t far_id = joint_far.id; // lower ID → visited first
    const uint32_t near_id = joint_near.id;
    steamrot::PartMap part_map;
    part_map.emplace(far_id, std::move(joint_far));
    part_map.emplace(near_id, std::move(joint_near));

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fi, part_map);

    // F[0] records the nearest joint (distance 1, ready)
    REQUIRE(fi.sockets[0].is_ready_to_connect == true);
    REQUIRE_THAT(fi.sockets[0].distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // F[1] has no candidate in range
    REQUIRE(fi.sockets[1].is_another_socket_near == false);
    REQUIRE(fi.sockets[1].is_ready_to_connect == false);

    // near joint is ready, far joint is proximity-only
    const steamrot::JointInstance &near_ref =
        std::get<steamrot::JointInstance>(part_map.at(near_id));
    REQUIRE(near_ref.sockets[0].is_ready_to_connect == true);

    const steamrot::JointInstance &far_ref =
        std::get<steamrot::JointInstance>(part_map.at(far_id));
    REQUIRE(far_ref.sockets[0].is_another_socket_near == true);
    REQUIRE(far_ref.sockets[0].is_ready_to_connect == false);
  }

  SECTION("three fragment sockets: only one pair ready to connect") {
    // Fragment sockets at world (0,0), (10,0), (20,0).
    // Three joints — each with one socket positioned so only the
    // F[0]↔J[0] pair is within connection threshold (1 unit), while
    // the others are proximity-only (5–9 units apart).
    //
    // Joint socket world positions: J[0]=(1,0), J[1]=(15,0), J[2]=(25,0).
    //
    // Distances:
    //   F[0](0,0)  ↔ J[0](1,0)   = 1   → ready
    //   F[0](0,0)  ↔ J[1](15,0)  = 15  → far
    //   F[0](0,0)  ↔ J[2](25,0)  = 25  → far
    //   F[1](10,0) ↔ J[0](1,0)   = 9   → near
    //   F[1](10,0) ↔ J[1](15,0)  = 5   → near (best for F[1])
    //   F[1](10,0) ↔ J[2](25,0)  = 15  → far
    //   F[2](20,0) ↔ J[0](1,0)   = 19  → far
    //   F[2](20,0) ↔ J[1](15,0)  = 5   → near
    //   F[2](20,0) ↔ J[2](25,0)  = 5   → near
    steamrot::FragmentInstance fi =
        builder.MakeFragmentInstance("fragment_three_sockets");
    REQUIRE(fi.sockets.size() == 3);
    fi.sockets[0].local_position = {0.f, 0.f};
    fi.sockets[1].local_position = {10.f, 0.f};
    fi.sockets[2].local_position = {20.f, 0.f};

    steamrot::JointInstance joint_0 =
        builder.MakeJointInstance("joint_one_socket");
    joint_0.sockets[0].local_position = {0.f, 0.f};
    joint_0.transform.translate({1.f, 0.f});

    steamrot::JointInstance joint_1 =
        builder.MakeJointInstance("joint_one_socket");
    joint_1.sockets[0].local_position = {0.f, 0.f};
    joint_1.transform.translate({15.f, 0.f});

    steamrot::JointInstance joint_2 =
        builder.MakeJointInstance("joint_one_socket");
    joint_2.sockets[0].local_position = {0.f, 0.f};
    joint_2.transform.translate({25.f, 0.f});

    const uint32_t id_0 = joint_0.id;
    const uint32_t id_1 = joint_1.id;
    const uint32_t id_2 = joint_2.id;
    steamrot::PartMap part_map;
    part_map.emplace(id_0, std::move(joint_0));
    part_map.emplace(id_1, std::move(joint_1));
    part_map.emplace(id_2, std::move(joint_2));

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fi, part_map);

    // count ready sockets on each side — only one pair should be ready
    int fragment_ready = 0;
    for (const steamrot::SocketData &s : fi.sockets)
      if (s.is_ready_to_connect)
        fragment_ready++;

    int joint_ready = 0;
    for (const auto &[id, variant] : part_map) {
      if (const auto *ji = std::get_if<steamrot::JointInstance>(&variant))
        if (ji->sockets[0].is_ready_to_connect)
          joint_ready++;
    }

    REQUIRE(fragment_ready == 1);
    REQUIRE(joint_ready == 1);

    // the ready pair must be F[0] and J[0]
    REQUIRE(fi.sockets[0].is_ready_to_connect == true);
    REQUIRE(fi.sockets[1].is_ready_to_connect == false);
    REQUIRE(fi.sockets[2].is_ready_to_connect == false);
    REQUIRE(std::get<steamrot::JointInstance>(part_map.at(id_0))
                .sockets[0].is_ready_to_connect == true);
    REQUIRE(std::get<steamrot::JointInstance>(part_map.at(id_1))
                .sockets[0].is_ready_to_connect == false);
    REQUIRE(std::get<steamrot::JointInstance>(part_map.at(id_2))
                .sockets[0].is_ready_to_connect == false);

    // non-ready sockets still reflect proximity state where applicable
    REQUIRE(fi.sockets[1].is_another_socket_near == true);
    REQUIRE(fi.sockets[2].is_another_socket_near == true);
  }
}

TEST_CASE("check_socket_collisions(JointInstance, PartMap) tests",
          "[unit][collision_grimoire_machina]") {
  steamrot::tests::TestPartLibrary part_library =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder(part_library);

  // Joint with one socket at local (0,0); identity transform → world (0,0)
  steamrot::JointInstance joint_instance =
      builder.MakeJointInstance("joint_one_socket");
  REQUIRE(joint_instance.sockets.size() == 1);
  joint_instance.sockets[0].local_position = {0.f, 0.f};

  SECTION("does not throw on empty PartMap") {
    steamrot::PartMap empty_map;
    REQUIRE_NOTHROW(
        steamrot::logic::collision::grimoire_machina::check_socket_collisions(
            joint_instance, empty_map));
    REQUIRE(joint_instance.sockets[0].is_another_socket_near == false);
    REQUIRE(joint_instance.sockets[0].is_ready_to_connect == false);
  }

  SECTION("no collision when fragment socket is far away") {
    steamrot::PartMap part_map =
        builder.MakePartMap({"fragment_one_socket"}, {});
    steamrot::FragmentInstance &fi =
        std::get<steamrot::FragmentInstance>(part_map.begin()->second);
    fi.sockets[0].local_position = {0.f, 0.f};
    fi.transform.translate({100.f, 0.f}); // world pos (100,0) — distance 100

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        joint_instance, part_map);

    REQUIRE(joint_instance.sockets[0].is_another_socket_near == false);
    REQUIRE(joint_instance.sockets[0].is_ready_to_connect == false);
    REQUIRE(fi.sockets[0].is_another_socket_near == false);
    REQUIRE(fi.sockets[0].is_ready_to_connect == false);
  }

  SECTION("is_another_socket_near is true when fragment socket is within "
          "proximity") {
    steamrot::PartMap part_map =
        builder.MakePartMap({"fragment_one_socket"}, {});
    steamrot::FragmentInstance &fi =
        std::get<steamrot::FragmentInstance>(part_map.begin()->second);
    fi.sockets[0].local_position = {0.f, 0.f};
    fi.transform.translate({5.f, 5.f}); // distance ≈ 7.07, within proximity (10)

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        joint_instance, part_map);

    REQUIRE(joint_instance.sockets[0].is_another_socket_near == true);
    REQUIRE(joint_instance.sockets[0].is_ready_to_connect == false);
    REQUIRE(fi.sockets[0].is_another_socket_near == true);
    REQUIRE(fi.sockets[0].is_ready_to_connect == false);
  }

  SECTION("is_ready_to_connect is true when fragment socket is within "
          "connection threshold") {
    steamrot::PartMap part_map =
        builder.MakePartMap({"fragment_one_socket"}, {});
    steamrot::FragmentInstance &fi =
        std::get<steamrot::FragmentInstance>(part_map.begin()->second);
    fi.sockets[0].local_position = {0.f, 0.f};
    fi.transform.translate({1.f, 1.f}); // distance ≈ 1.41, within connection (2.5)

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        joint_instance, part_map);

    REQUIRE(joint_instance.sockets[0].is_another_socket_near == true);
    REQUIRE(joint_instance.sockets[0].is_ready_to_connect == true);
    REQUIRE(fi.sockets[0].is_another_socket_near == true);
    REQUIRE(fi.sockets[0].is_ready_to_connect == true);
  }

  SECTION("most proximal fragment wins when two fragments are within range") {
    // Build two fragment instances manually so we control IDs (lower ID
    // visited first by std::map).
    steamrot::FragmentInstance frag_far =
        builder.MakeFragmentInstance("fragment_one_socket");
    frag_far.sockets[0].local_position = {0.f, 0.f};
    frag_far.transform.translate({8.f, 0.f}); // distance 8 — proximity only

    steamrot::FragmentInstance frag_near =
        builder.MakeFragmentInstance("fragment_one_socket");
    frag_near.sockets[0].local_position = {0.f, 0.f};
    frag_near.transform.translate({1.f, 0.f}); // distance 1 — ready to connect

    const uint32_t far_id = frag_far.id; // lower ID → visited first
    const uint32_t near_id = frag_near.id;
    steamrot::PartMap part_map;
    part_map.emplace(far_id, std::move(frag_far));
    part_map.emplace(near_id, std::move(frag_near));

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        joint_instance, part_map);

    // joint socket must reflect the nearest candidate
    REQUIRE(joint_instance.sockets[0].is_ready_to_connect == true);
    REQUIRE(joint_instance.sockets[0].distance_to_nearest_socket.has_value());
    REQUIRE_THAT(
        joint_instance.sockets[0].distance_to_nearest_socket.value(),
        Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // near fragment socket must be ready to connect
    const steamrot::FragmentInstance &near_ref =
        std::get<steamrot::FragmentInstance>(part_map.at(near_id));
    REQUIRE(near_ref.sockets[0].is_ready_to_connect == true);

    // far fragment socket must be proximity-only (not ready)
    const steamrot::FragmentInstance &far_ref =
        std::get<steamrot::FragmentInstance>(part_map.at(far_id));
    REQUIRE(far_ref.sockets[0].is_another_socket_near == true);
    REQUIRE(far_ref.sockets[0].is_ready_to_connect == false);
  }

  SECTION("stale state from a previous tick is cleared at the start of each "
          "call") {
    steamrot::PartMap part_map =
        builder.MakePartMap({"fragment_one_socket"}, {});
    steamrot::FragmentInstance &fi =
        std::get<steamrot::FragmentInstance>(part_map.begin()->second);
    fi.sockets[0].local_position = {0.f, 0.f};

    // first tick: sockets close — state is set
    fi.transform.translate({1.f, 1.f});
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        joint_instance, part_map);
    REQUIRE(joint_instance.sockets[0].is_ready_to_connect == true);
    REQUIRE(fi.sockets[0].is_ready_to_connect == true);

    // second tick: fragment moves far away
    fi.transform = sf::Transform::Identity;
    fi.transform.translate({100.f, 0.f});
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        joint_instance, part_map);

    // stale state must have been cleared by the internal reset
    REQUIRE(joint_instance.sockets[0].is_another_socket_near == false);
    REQUIRE(joint_instance.sockets[0].is_ready_to_connect == false);
    REQUIRE(fi.sockets[0].is_another_socket_near == false);
    REQUIRE(fi.sockets[0].is_ready_to_connect == false);
  }
}

TEST_CASE("check_socket_collisions(JointInstance, PartMap) multi-socket tests",
          "[unit][collision_grimoire_machina]") {
  steamrot::tests::TestPartLibrary part_library =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder(part_library);

  SECTION("two joint sockets: each finds its own nearest fragment candidate") {
    // Joint sockets at world (0,0) and (20,0).
    // Two fragments in PartMap with sockets at world (1,0) and (19,0).
    steamrot::JointInstance ji =
        builder.MakeJointInstance("joint_two_sockets");
    REQUIRE(ji.sockets.size() == 2);
    ji.sockets[0].local_position = {0.f, 0.f};
    ji.sockets[1].local_position = {20.f, 0.f};

    steamrot::FragmentInstance frag_a =
        builder.MakeFragmentInstance("fragment_one_socket");
    frag_a.sockets[0].local_position = {0.f, 0.f};
    frag_a.transform.translate({1.f, 0.f});

    steamrot::FragmentInstance frag_b =
        builder.MakeFragmentInstance("fragment_one_socket");
    frag_b.sockets[0].local_position = {0.f, 0.f};
    frag_b.transform.translate({19.f, 0.f});

    const uint32_t a_id = frag_a.id;
    const uint32_t b_id = frag_b.id;
    steamrot::PartMap part_map;
    part_map.emplace(a_id, std::move(frag_a));
    part_map.emplace(b_id, std::move(frag_b));

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        ji, part_map);

    REQUIRE(ji.sockets[0].is_ready_to_connect == true);
    REQUIRE_THAT(ji.sockets[0].distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));
    REQUIRE(ji.sockets[1].is_ready_to_connect == true);
    REQUIRE_THAT(ji.sockets[1].distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));
  }

  SECTION("two joint sockets: most proximal fragment wins per socket") {
    // Joint sockets at world (0,0) and (20,0) — far apart.
    // Both fragments clustered near x=0: frag_far at x=8, frag_near at x=1.
    // J[0] should prefer frag_near; J[1] has no candidate in range.
    steamrot::JointInstance ji =
        builder.MakeJointInstance("joint_two_sockets");
    REQUIRE(ji.sockets.size() == 2);
    ji.sockets[0].local_position = {0.f, 0.f};
    ji.sockets[1].local_position = {20.f, 0.f};

    steamrot::FragmentInstance frag_far =
        builder.MakeFragmentInstance("fragment_one_socket");
    frag_far.sockets[0].local_position = {0.f, 0.f};
    frag_far.transform.translate({8.f, 0.f});

    steamrot::FragmentInstance frag_near =
        builder.MakeFragmentInstance("fragment_one_socket");
    frag_near.sockets[0].local_position = {0.f, 0.f};
    frag_near.transform.translate({1.f, 0.f});

    const uint32_t far_id = frag_far.id;
    const uint32_t near_id = frag_near.id;
    steamrot::PartMap part_map;
    part_map.emplace(far_id, std::move(frag_far));
    part_map.emplace(near_id, std::move(frag_near));

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        ji, part_map);

    // J[0] records nearest fragment (distance 1, ready)
    REQUIRE(ji.sockets[0].is_ready_to_connect == true);
    REQUIRE_THAT(ji.sockets[0].distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(1.f, 0.01f));

    // J[1] has no candidate in range
    REQUIRE(ji.sockets[1].is_another_socket_near == false);
    REQUIRE(ji.sockets[1].is_ready_to_connect == false);
  }

  SECTION("three joint sockets: only one pair ready to connect") {
    // Joint sockets at world (0,0), (10,0), (20,0).
    // Three fragments — only the J[0]↔F[0] pair is within connection
    // threshold; others are proximity-only.
    //
    // Fragment socket world positions: F[0]=(1,0), F[1]=(15,0), F[2]=(25,0).
    steamrot::JointInstance ji =
        builder.MakeJointInstance("joint_three_sockets");
    REQUIRE(ji.sockets.size() == 3);
    ji.sockets[0].local_position = {0.f, 0.f};
    ji.sockets[1].local_position = {10.f, 0.f};
    ji.sockets[2].local_position = {20.f, 0.f};

    steamrot::FragmentInstance frag_0 =
        builder.MakeFragmentInstance("fragment_one_socket");
    frag_0.sockets[0].local_position = {0.f, 0.f};
    frag_0.transform.translate({1.f, 0.f});

    steamrot::FragmentInstance frag_1 =
        builder.MakeFragmentInstance("fragment_one_socket");
    frag_1.sockets[0].local_position = {0.f, 0.f};
    frag_1.transform.translate({15.f, 0.f});

    steamrot::FragmentInstance frag_2 =
        builder.MakeFragmentInstance("fragment_one_socket");
    frag_2.sockets[0].local_position = {0.f, 0.f};
    frag_2.transform.translate({25.f, 0.f});

    const uint32_t id_0 = frag_0.id;
    const uint32_t id_1 = frag_1.id;
    const uint32_t id_2 = frag_2.id;
    steamrot::PartMap part_map;
    part_map.emplace(id_0, std::move(frag_0));
    part_map.emplace(id_1, std::move(frag_1));
    part_map.emplace(id_2, std::move(frag_2));

    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        ji, part_map);

    // only one joint socket should be ready to connect
    int joint_ready = 0;
    for (const steamrot::SocketData &s : ji.sockets)
      if (s.is_ready_to_connect)
        joint_ready++;

    int frag_ready = 0;
    for (const auto &[id, variant] : part_map) {
      if (const auto *fi = std::get_if<steamrot::FragmentInstance>(&variant))
        if (fi->sockets[0].is_ready_to_connect)
          frag_ready++;
    }

    REQUIRE(joint_ready == 1);
    REQUIRE(frag_ready == 1);

    // specifically J[0] and F[0] (frag_0)
    REQUIRE(ji.sockets[0].is_ready_to_connect == true);
    REQUIRE(ji.sockets[1].is_ready_to_connect == false);
    REQUIRE(ji.sockets[2].is_ready_to_connect == false);
    REQUIRE(std::get<steamrot::FragmentInstance>(part_map.at(id_0))
                .sockets[0].is_ready_to_connect == true);
    REQUIRE(std::get<steamrot::FragmentInstance>(part_map.at(id_1))
                .sockets[0].is_ready_to_connect == false);
    REQUIRE(std::get<steamrot::FragmentInstance>(part_map.at(id_2))
                .sockets[0].is_ready_to_connect == false);

    // non-ready joint sockets still reflect proximity state where applicable
    REQUIRE(ji.sockets[1].is_another_socket_near == true);
    REQUIRE(ji.sockets[2].is_another_socket_near == true);
  }
}
