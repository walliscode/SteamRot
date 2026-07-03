/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SocketConfig struct, which defines the number and
/// layout of sockets on a Joint.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <cstdint>

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
  /// @brief Default constructor
  /////////////////////////////////////////////////
  SocketConfig() = default;

  /////////////////////////////////////////////////
  /// @brief Construct a SocketConfig with the given parameters.
  ///
  /// @param count [TODO:parameter]
  /// @param radius [TODO:parameter]
  /// @param min_gap [TODO:parameter]
  /// @param fixed_socket_angle [TODO:parameter]
  /// @param has_fixed_socket [TODO:parameter]
  /// @param rotation_arc_min [TODO:parameter]
  /// @param rotation_arc_max [TODO:parameter]
  /////////////////////////////////////////////////
  SocketConfig(uint8_t count, float radius, float min_gap,
               float fixed_socket_angle, bool has_fixed_socket,
               float rotation_arc_min, float rotation_arc_max)
      : socket_count(count), radius(radius), min_gap(min_gap),
        fixed_socket_angle(fixed_socket_angle),
        has_fixed_socket(has_fixed_socket), rotation_arc_min(rotation_arc_min),
        rotation_arc_max(rotation_arc_max) {}

  /////////////////////////////////////////////////
  /// @brief Total number of sockets including the fixed anchor (if present).
  /////////////////////////////////////////////////
  uint8_t socket_count{0};

  /////////////////////////////////////////////////
  /// @brief Distance of all sockets from the Joint's local origin.
  /////////////////////////////////////////////////
  float radius{10.f};

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

  /////////////////////////////////////////////////
  /// @brief Minimum rotation angle (degrees, 0–360) representing the mechanical
  /// lower limit of the socket ring. Used by positioning Logic to clamp the
  /// ring's rotation. World-space orientation of the joint is encoded in the
  /// instance transform, not in this value.
  /////////////////////////////////////////////////
  float rotation_arc_min{0.f};

  /////////////////////////////////////////////////
  /// @brief Maximum rotation angle (degrees, 0–360) representing the mechanical
  /// upper limit of the socket ring. Used by positioning Logic to clamp the
  /// ring's rotation. World-space orientation of the joint is encoded in the
  /// instance transform, not in this value.
  /////////////////////////////////////////////////
  float rotation_arc_max{360.f};
};
} // namespace steamrot
