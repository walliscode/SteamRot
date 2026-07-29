/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for collision checking of GrimoireMachina elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace logic::collision::grimoire_machina;

TEST_CASE("reset_socket_proximity_state(PartGraph) tests",
          "[unit][collision_grimoire_machina]") {
  SECTION("does not throw on an empty PartGraph") {
    PartGraph empty_map;
    REQUIRE_NOTHROW(reset_socket_proximity_state(empty_map));
  }
}

} // namespace steamrot::tests
