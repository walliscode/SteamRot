/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the Fragment struct and related functionality.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "Vector2fEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("Socket struct tests", "[Types]") {

  SECTION("Socket constructor initializes members correctly") {
    sf::Vector2f local_pos(1.0f, 2.0f);
    sf::Vector2f align_vec(3.0f, 4.0f);
    Socket socket(local_pos, align_vec);
    REQUIRE_THAT(socket.local_position, EqualsVector2f(local_pos));
    REQUIRE_THAT(socket.alignment_vector,
                 EqualsVector2f(align_vec.normalized()));
  }
}
} // namespace steamrot::tests
