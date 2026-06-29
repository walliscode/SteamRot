/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the fragment library
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "fragment_library.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace parts;

TEST_CASE("CreateRectangleFragmentWith2Sockets creates the correct fragment",
          "[fragment_library]") {

  const Fragment &fragment = FragmentRectangleWith2Sockets;

  // test name
  REQUIRE(fragment.name == "FragmentRectangleWith2Sockets");
  // test views
  REQUIRE(fragment.positioning_views.size() == 1);
  REQUIRE(fragment.positioning_views[ViewDirection::Front].getVertexCount() ==
          6);

  // test sockets
  REQUIRE(fragment.sockets.size() == 2);
  REQUIRE(fragment.sockets[0].local_position == sf::Vector2f(0, 5));
  REQUIRE(fragment.sockets[0].alignment_vector == sf::Vector2f(1, 0));
  REQUIRE(fragment.sockets[1].local_position == sf::Vector2f(50, 5));
  REQUIRE(fragment.sockets[1].alignment_vector == sf::Vector2f(-1, 0));
}
} // namespace steamrot::tests
