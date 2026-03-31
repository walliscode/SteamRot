/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CheckMouseOver overloads in collision::mouse
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_mouse.h"
#include "MachinaFormScaffold.h"
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CheckMouseOver GrowthPoint sets is_mouse_over correctly",
          "[unit][collision][mouse]") {

  SECTION("Mouse is over growth point") {
    steamrot::GrowthPoint growth_point;
    growth_point.origin.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(105, 105);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      growth_point);
    REQUIRE(growth_point.is_mouse_over == true);
  }

  SECTION("Mouse is not over growth point") {
    steamrot::GrowthPoint growth_point;
    growth_point.origin.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(200, 200);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      growth_point);
    REQUIRE(growth_point.is_mouse_over == false);
  }
}

TEST_CASE("CheckMouseOver Socket sets is_mouse_over correctly",
          "[unit][collision][mouse]") {

  SECTION("Mouse is over socket") {
    steamrot::Socket socket;
    socket.circle.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(103, 103);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, socket);
    REQUIRE(socket.is_mouse_over == true);
  }

  SECTION("Mouse is not over socket") {
    steamrot::Socket socket;
    socket.circle.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(200, 200);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, socket);
    REQUIRE(socket.is_mouse_over == false);
  }
}

TEST_CASE("CheckMouseOver FragmentInstance sets socket is_mouse_over correctly",
          "[unit][collision][mouse]") {

  steamrot::Fragment fragment;

  SECTION("Mouse is over a socket on the fragment") {
    steamrot::Socket socket;
    socket.circle.setPosition({100.f, 100.f});
    steamrot::FragmentInstance fragment_instance{fragment, {}, {socket}};
    sf::Vector2i mouse_position(103, 103);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      fragment_instance);
    REQUIRE(fragment_instance.sockets[0].is_mouse_over == true);
  }

  SECTION("Mouse is not over any socket on the fragment") {
    steamrot::Socket socket;
    socket.circle.setPosition({100.f, 100.f});
    steamrot::FragmentInstance fragment_instance{fragment, {}, {socket}};
    sf::Vector2i mouse_position(200, 200);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      fragment_instance);
    REQUIRE(fragment_instance.sockets[0].is_mouse_over == false);
  }

  SECTION("Fragment with no sockets does not crash") {
    steamrot::FragmentInstance fragment_instance{fragment, {}, {}};
    sf::Vector2i mouse_position(100, 100);
    REQUIRE_NOTHROW(steamrot::logic::collision::mouse::CheckMouseOver(
        mouse_position, fragment_instance));
  }
}

TEST_CASE("CheckMouseOver JointInstance sets socket is_mouse_over correctly",
          "[unit][collision][mouse]") {

  steamrot::Joint joint;

  SECTION("Mouse is over a socket on the joint") {
    steamrot::Socket socket;
    socket.circle.setPosition({100.f, 100.f});
    steamrot::JointInstance joint_instance{joint, {}, {socket}};
    sf::Vector2i mouse_position(103, 103);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      joint_instance);
    REQUIRE(joint_instance.sockets[0].is_mouse_over == true);
  }

  SECTION("Mouse is not over any socket on the joint") {
    steamrot::Socket socket;
    socket.circle.setPosition({100.f, 100.f});
    steamrot::JointInstance joint_instance{joint, {}, {socket}};
    sf::Vector2i mouse_position(200, 200);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      joint_instance);
    REQUIRE(joint_instance.sockets[0].is_mouse_over == false);
  }

  SECTION("Joint with no sockets does not crash") {
    steamrot::JointInstance joint_instance{joint, {}, {}};
    sf::Vector2i mouse_position(100, 100);
    REQUIRE_NOTHROW(steamrot::logic::collision::mouse::CheckMouseOver(
        mouse_position, joint_instance));
  }
}
