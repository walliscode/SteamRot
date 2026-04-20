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

TEST_CASE("check_socket_collisions(SocketData, SocketData) tests") {

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
    REQUIRE_THAT(socket_two_data.distance_to_nearest_socket.value(),
                 Catch::Matchers::WithinAbsMatcher(7.07f, 0.01f));

    // arrange - move sockets apart
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
    // arrange - move sockets apart
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
TEST_CASE("check_socket_collsions(FragmentInstance,JointInstance) tests with "
          "single socket collisions") {

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
    // arrange - move sockets apart
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
    // arrange - move sockets apart
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
          "multiple socket collisions") {
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

// TEST_CASE("check_socket_collisions tests") {
//
//   // arrange
//   steamrot::tests::TestPartLibrary part_library =
//       steamrot::tests::TestPartLibrary::Create();
//   steamrot::tests::PartLibraryBuilder builder(part_library);
//
//   steamrot::FragmentInstance fragment_instance =
//       builder.MakeFragmentInstance("fragment_one_socket");
//   REQUIRE(fragment_instance.sockets.size() == 1);
//   steamrot::SocketData &fragment_socket_data =
//   fragment_instance.sockets[0];
//
//   steamrot::JointInstance joint_instance =
//       builder.MakeJointInstance("joint_one_socket");
//   steamrot::PartMap empty_part_map;
//   steamrot::PartMap part_map_one =
//       builder.MakePartMap({"fragment_one_socket"}, {});
//
//   SECTION("socket does not collide with empty PartMap") {
//     // act
//     steamrot::logic::collision::grimoire_machina::check_socket_collisions(
//         fragment_instance, part_map_one);
//
//     // assert
//     steamrot::SocketData &socket_data = fragment_instance.sockets[0];
//     REQUIRE(socket_data.state == fragment_socket_data.state);
//     REQUIRE(socket_data.is_mouse_over == false);
//   }
// }
