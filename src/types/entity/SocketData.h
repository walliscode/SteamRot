/////////////////////////////////////////////////
/// @file
/// @brief Declarartion of the SocketConfig struct. No implementation is needed
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/System/Vector2.hpp>

namespace steamrot {

struct SocketData {

  /////////////////////////////////////////////////
  /// @brief Constructor for Socket. Initializes local_position and
  /// alignment_vector.
  ///
  /// @param local_pos Vector2f representing the local position of the socket in
  /// the part's own coordinate space.
  /// @param align_vec Vector2f representing the alignment vector/axis in the
  /// part's own coordinate space. It will be normalized.
  /////////////////////////////////////////////////
  SocketData(const sf::Vector2f &local_pos, const sf::Vector2f &align_vec)
      : local_position{local_pos}, alignment_vector(align_vec.normalized()) {}

  /////////////////////////////////////////////////
  /// @brief Constructor for SocketData. Initializes local_position and
  /// alignment_vector from another SocketData instance.
  ///
  /// @param other SocketData instance to copy from.
  /////////////////////////////////////////////////
  SocketData(const SocketData &other)
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
} // namespace steamrot
