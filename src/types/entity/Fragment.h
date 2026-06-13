/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Fragment struct. No implementation is needed.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Part.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

namespace steamrot {

struct Socket {

  /////////////////////////////////////////////////
  /// @brief Constructor for Socket. Initializes local_position and
  /// alignment_vector.
  ///
  /// @param local_pos Vector2f representing the local position of the socket in
  /// the part's own coordinate space.
  /// @param align_vec Vector2f representing the alignment vector/axis in the
  /// part's own coordinate space. It will be normalized.
  /////////////////////////////////////////////////
  Socket(const sf::Vector2f &local_pos, const sf::Vector2f &align_vec)
      : local_position{local_pos}, alignment_vector(align_vec.normalized()) {}

  Socket(const Socket &other)
      : local_position(other.local_position),
        alignment_vector(other.alignment_vector) {}

  /////////////////////////////////////////////////
  /// @brief local position of the socket in the part's own coordinate space.
  /////////////////////////////////////////////////
  const sf::Vector2f local_position;

  /////////////////////////////////////////////////
  /// @brief along with the local position, creates an alignment vector/axis in
  /// the part's own coordinate space.
  /////////////////////////////////////////////////
  const sf::Vector2f alignment_vector;
};
/////////////////////////////////////////////////
/// @struct Fragment
/// @brief Contains the data for a Fragment.
///
/// Inherits common part data (name, positioning_views) from Part.
/// Stores explicit local socket positions in the part's own coordinate space.
/////////////////////////////////////////////////
struct Fragment : Part {
  /////////////////////////////////////////////////
  /// @brief Local positions of the fragment's sockets, in the part's own
  /// coordinate space.
  /////////////////////////////////////////////////
  std::vector<Socket> sockets;
};

} // namespace steamrot
