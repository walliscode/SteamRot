/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for collision checking of GrimoireMachina elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include "grimoire_machina_test_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("check_socket_collisions tests") {

  // arrange

  // create Fragment and pass to FragmentInstance
  steamrot::Fragment fragment =
      steamrot::tests::MakeFragmentWithOriginTriangle();
  steamrot::FragmentInstance fragment_instance{&fragment};

  // FragmentInstance assertions
  REQUIRE(fragment_instance.sockets.size() == 1);
  REQUIRE(fragment_instance.sockets[0].local_position ==
          sf::Vector2f{5.f, 5.f});
  REQUIRE(fragment_instance.sockets[0].state.state ==
          steamrot::SocketState::State::Available);
  REQUIRE(fragment_instance.sockets[0].state.is_mouse_over == false);

  // create PartMaps
  steamrot::PartMap empty_part_map;
  steamrot::PartMap one_fragment_part_map;
  steamrot::PartMap one_joint_part_map;

  SECTION("check_socket_collisions does nothing with empty PartMap and does "
          "not throw") {
    // act
    steamrot::logic::collision::grimoire_machina::check_socket_collisions(
        fragment_instance, empty_part_map);
    // assert
    // (no assertions yet since function is not implemented)
  }
}
