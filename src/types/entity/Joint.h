/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Joint struct. No implementation is needed.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Part.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SocketConfig
/// @brief Procedural description of a Joint's socket layout.
///
/// Sockets are distributed evenly within the arc [arc_min, arc_max] at the
/// given radius from the Joint's local origin. When arc_max - arc_min equals
/// 360 degrees (a full circle), sockets are placed at equal angular intervals
/// with no duplicate at the wrap-around point.
///
/// An optional fixed anchor socket (socket index 0 when has_fixed_socket is
/// true) sits at fixed_socket_angle and is excluded from current_rotation.
/// Remaining sockets rotate with the ring.
/////////////////////////////////////////////////
struct SocketConfig {
  /////////////////////////////////////////////////
  /// @brief Total number of sockets including the fixed anchor (if present).
  /////////////////////////////////////////////////
  int socket_count{0};

  /////////////////////////////////////////////////
  /// @brief Distance of all sockets from the Joint's local origin.
  /////////////////////////////////////////////////
  float radius{10.f};

  /////////////////////////////////////////////////
  /// @brief Start angle (degrees) of the socket distribution arc.
  /////////////////////////////////////////////////
  float arc_min{0.f};

  /////////////////////////////////////////////////
  /// @brief End angle (degrees) of the socket distribution arc.
  ///
  /// When arc_max - arc_min == 360, sockets are distributed over a full
  /// circle without duplicating the start/end position.
  /////////////////////////////////////////////////
  float arc_max{360.f};

  /////////////////////////////////////////////////
  /// @brief Minimum angular separation (degrees) between adjacent rotatable
  /// sockets.
  /////////////////////////////////////////////////
  float min_gap{0.f};

  /////////////////////////////////////////////////
  /// @brief Angle (degrees) of the fixed anchor socket (socket index 0).
  ///
  /// Only used when has_fixed_socket is true.
  /////////////////////////////////////////////////
  float fixed_socket_angle{0.f};

  /////////////////////////////////////////////////
  /// @brief Whether socket index 0 is a fixed anchor that does not rotate.
  /////////////////////////////////////////////////
  bool has_fixed_socket{false};
};

/////////////////////////////////////////////////
/// @struct Joint
/// @brief Contains the data for a Joint.
///
/// Inherits common part data (name, movement_views) from Part.
/// Sockets are described procedurally via a SocketConfig rather than as
/// explicit positions, allowing them to rotate around the Joint's centre.
/////////////////////////////////////////////////
struct Joint : Part {
  /////////////////////////////////////////////////
  /// @brief Procedural socket layout description for this Joint.
  /////////////////////////////////////////////////
  SocketConfig socket_config;
};

} // namespace steamrot
