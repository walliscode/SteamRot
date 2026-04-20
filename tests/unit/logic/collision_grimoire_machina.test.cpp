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

    // reset state (at PartMap level this is handled by reset_socket_proximity_state)
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

    // reset state (at PartMap level this is handled by reset_socket_proximity_state)
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

  SECTION(
      "only a single socket pair is set if multiple options are available") {
    // arrange
    fragment_instance.transform.translate({0.f, 0.f});
    joint_instance.transform.translate({1.f, 1.f});
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, joint_instance);
    // assert - only one pair of sockets should be colliding, but we don't
    // care which one since the logic doesn't guarantee a specific pairing
    int fragment_sockets_ready_to_connect = 0;
    for (steamrot::SocketData &socket_data : fragment_instance.sockets) {
      if (socket_data.is_ready_to_connect) {
        fragment_sockets_ready_to_connect++;
      }
    }
    int joint_sockets_ready_to_connect = 0;
    for (steamrot::SocketData &socket_data : joint_instance.sockets) {
      if (socket_data.is_ready_to_connect) {
        joint_sockets_ready_to_connect++;
      }
    }
    REQUIRE(fragment_sockets_ready_to_connect == 1);
    REQUIRE(joint_sockets_ready_to_connect == 1);
  }
}

TEST_CASE("reset_socket_proximity_state(PartMap) tests",
          "[unit][collision_grimoire_machina]") {
  steamrot::tests::TestPartLibrary part_library =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder(part_library);

  SECTION("does not throw on an empty PartMap") {
    steamrot::PartMap empty_map;
    REQUIRE_NOTHROW(
        steamrot::logic::collision::grimoire_machina::reset_socket_proximity_state(
            empty_map));
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
    ji.transform.translate({5.f, 5.f}); // distance ≈ 7.07, within proximity (10)

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
    ji.transform.translate({1.f, 1.f}); // distance ≈ 1.41, within connection (2.5)

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
    REQUIRE(fragment_instance.sockets[0].distance_to_nearest_socket.has_value());
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
