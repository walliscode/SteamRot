/////////////////////////////////////////////////
/// @file
/// @brief Header-only utility functions for SocketConfig computations.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Joint.h"
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <numbers>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Degrees-to-radians conversion factor used by socket computations.
/////////////////////////////////////////////////
inline constexpr float k_socket_deg_to_rad =
    std::numbers::pi_v<float> / 180.f;

/////////////////////////////////////////////////
/// @brief Compute the local-space position of a single socket on a Joint.
///
/// Fixed anchor sockets (socket index 0 when SocketConfig::has_fixed_socket is
/// true) are placed at SocketConfig::fixed_socket_angle and are not affected by
/// @p current_rotation. All other sockets are distributed evenly within the
/// arc [arc_min, arc_max] and rotated by @p current_rotation.
///
/// When the arc spans exactly 360 degrees, sockets are distributed with equal
/// spacing and no duplicate at the wrap-around point.
///
/// Angles are in degrees; the resulting position uses SFML's coordinate system
/// (y increases downward).
///
/// @param config           SocketConfig describing the Joint's socket layout.
/// @param socket_index     Index of the socket to compute (0-based).
/// @param current_rotation Current rotation of the socket ring in degrees.
/// @return Local-space position of the requested socket.
/////////////////////////////////////////////////
inline sf::Vector2f ComputeSocketLocalPos(const SocketConfig &config,
                                          size_t socket_index,
                                          float current_rotation) {
  // Fixed anchor socket: sits at fixed_socket_angle and does not rotate
  if (config.has_fixed_socket && socket_index == 0) {
    const float rad = config.fixed_socket_angle * k_socket_deg_to_rad;
    return {config.radius * std::cos(rad), config.radius * std::sin(rad)};
  }

  // Offset index to account for fixed socket occupying slot 0
  const size_t rot_index =
      config.has_fixed_socket ? socket_index - 1 : socket_index;
  const size_t rot_count = config.has_fixed_socket
                               ? static_cast<size_t>(config.socket_count) - 1
                               : static_cast<size_t>(config.socket_count);

  float base_angle_deg;
  if (rot_count <= 1) {
    base_angle_deg = (config.arc_min + config.arc_max) / 2.f;
  } else {
    const float span = config.arc_max - config.arc_min;
    // Full circle: avoid duplicating the start/end position
    if (std::abs(span - 360.f) < 1e-3f) {
      base_angle_deg = config.arc_min +
                       static_cast<float>(rot_index) * 360.f /
                           static_cast<float>(rot_count);
    } else {
      base_angle_deg =
          config.arc_min + static_cast<float>(rot_index) * span /
                               static_cast<float>(rot_count - 1);
    }
  }

  const float total_rad = (base_angle_deg + current_rotation) * k_socket_deg_to_rad;
  return {config.radius * std::cos(total_rad),
          config.radius * std::sin(total_rad)};
}

} // namespace steamrot
