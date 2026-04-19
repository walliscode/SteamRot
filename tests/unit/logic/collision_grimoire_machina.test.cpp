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

    REQUIRE(socket_two_data.state == steamrot::SocketState::Available);
    REQUIRE(socket_two_data.is_mouse_over == false);
    REQUIRE(socket_two_data.is_another_socket_near == false);
    REQUIRE(socket_two_data.is_ready_to_connect == false);
    REQUIRE_THAT(socket_two_data.local_position,
                 steamrot::tests::EqualsVector2f({0.f, 0.f}));
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
    REQUIRE(socket_two_data.is_ready_to_connect == false);
    REQUIRE(socket_two_data.is_another_socket_near == true);
    REQUIRE(socket_one_data.is_ready_to_connect == false);
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
    REQUIRE(socket_two_data.is_ready_to_connect == false);
    REQUIRE(socket_two_data.is_another_socket_near == true);
    REQUIRE(socket_one_data.is_ready_to_connect == false);

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
}
// TEST_CASE("check_socket_collsions(FragmentInstance,JointInstance) tests") {
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
//   REQUIRE(joint_instance.sockets.size() == 1);
//   steamrot::SocketData &joint_socket_data = joint_instance.sockets[0];
//
//   SECTION("sockets do not collide when far apart") {
//     // arrange
//     fragment_instance.transform.translate({0.f, 0.f});
//     joint_instance.transform.translate({100.f, 100.f});
//     // act
//     steamrot::logic::collision::grimoire_machina::check_socket_collisions(
//         fragment_instance, joint_instance);
//     // assert
//     steamrot::SocketData &socket_data = fragment_instance.sockets[0];
//     REQUIRE(socket_data.state == fragment_socket_data.state);
//     REQUIRE(socket_data.is_mouse_over == false);
//   }
// }
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
