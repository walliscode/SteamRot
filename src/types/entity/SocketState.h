/////////////////////////////////////////////////
/// @file
/// @brief Implentation of the SocketState struct and its derivatives
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <map>
#include <optional>
#include <variant>

namespace steamrot {

enum class SocketConnectionState {
  Available, ///< Socket exists and can connect
  Connected, ///< Socket is currently connected
  Blocked    ///< Socket exists but can't be used (e.g., edge of canvas)
};

/////////////////////////////////////////////////
/// @struct SocketConnection
/// @brief Identifies the peer endpoint of an established socket connection.
///
/// When a socket's @c state is @c SocketState::Connected, its
/// @c SocketData::connected_to field holds a @c SocketConnection that names
/// the peer part by stable ID and the peer socket by index. Both ends of a
/// connection store a reciprocal @c SocketConnection so the graph can be
/// traversed from either direction without a central connection list.
/////////////////////////////////////////////////
struct SocketConnection {
  /////////////////////////////////////////////////
  /// @brief Stable ID of the peer PartInstance (matches
  /// JointInstance::id or FragmentInstance::id).
  /////////////////////////////////////////////////
  uint32_t peer_part_id{0};

  /////////////////////////////////////////////////
  /// @brief Stable ID of the connected socket on the peer instance (matches
  /// the key in the peer instance's @c sockets map).
  /////////////////////////////////////////////////
  uint32_t peer_socket_id{0};
};

/////////////////////////////////////////////////
/// @struct SocketData
/// @brief Bundles the local position and mutable runtime state for a single
/// socket on a placed part instance.
///
/// @c local_position is initialised at construction from the owning Part
/// definition and may be updated by positioning Logic (e.g. to rotate the
/// socket ring of a JointInstance). @c state holds the mutable connection
/// status and hover flag that are updated each tick by Logic.
/////////////////////////////////////////////////
struct SocketState {

  /////////////////////////////////////////////////
  /// @brief Mutable runtime state of this socket (connection status, hover).
  /////////////////////////////////////////////////
  SocketConnectionState connection_state{SocketConnectionState::Available};

  /////////////////////////////////////////////////
  /// @brief Boolean flag set to true if the mouse is currently hovering over
  /// this socket.
  /////////////////////////////////////////////////
  bool is_mouse_over{false};

  /////////////////////////////////////////////////
  /// @brief Boolean flag set to true if another socket is within a
  /// pre_determined radius
  /////////////////////////////////////////////////
  bool is_another_socket_near{false};

  /////////////////////////////////////////////////
  /// @brief Boolean flag set to true if this socket is within the snap radius
  /// of another
  /////////////////////////////////////////////////
  bool is_ready_to_connect{false};

  /////////////////////////////////////////////////
  /// @brief if is_another_socket_near is true, this holds the distance to the
  /// nearest socket
  /////////////////////////////////////////////////
  std::optional<float> distance_to_nearest_socket{std::nullopt};

  /////////////////////////////////////////////////
  /// @brief Proximity brightness scale in the range [0, 255].
  ///
  /// Set when @c is_another_socket_near is true. The value maps the distance
  /// to the nearest socket onto a [0, 255] range: 255 at the connection
  /// threshold (closest) and 0 at the outer proximity threshold (furthest
  /// within range). Reset to @c std::nullopt by the collision system whenever
  /// @c is_another_socket_near is false.
  /////////////////////////////////////////////////
  std::optional<uint8_t> proximity_scale{std::nullopt};

  /////////////////////////////////////////////////
  /// @brief When @c state is @c SocketState::Connected, holds the peer
  /// endpoint of this connection. @c std::nullopt otherwise.
  ///
  /// Both ends of every connection store a reciprocal @c SocketConnection so
  /// the graph can be traversed from either direction without a central
  /// connection list.
  /////////////////////////////////////////////////
  std::optional<SocketConnection> connected_to{std::nullopt};
};

struct FragmentSocketState : public SocketState {

  /////////////////////////////////////////////////
  /// @brief Constructs a FragmentSocketState with the given local position.
  ///
  /// @param local_pos Reference to the local position of the socket in the
  /// Fragment's coordinate space.
  /////////////////////////////////////////////////
  explicit FragmentSocketState(const sf::Vector2f &local_pos)
      : local_position{local_pos} {};

  /////////////////////////////////////////////////
  /// @brief References a socket on the Fragment
  ///
  /// Sockets on a Fragment are fixed in the Fragment's own coordinate space, so
  /// this does not need to change. Changes in position can be handled by the
  /// FragmentInstance's transform.
  /////////////////////////////////////////////////
  const sf::Vector2f &local_position;
};

struct JointSocketState : public SocketState {

  /////////////////////////////////////////////////
  /// @brief Shows the local position of the socket in the Joint's coordinate
  /// space.
  ///
  /// This is mutable because the socket ring of a JointInstance can be rotated
  /////////////////////////////////////////////////
  sf::Vector2f local_position{0.f, 0.f};
};

using SocketMap = std::variant<std::map<uint32_t, FragmentSocketState>,
                               std::map<uint32_t, JointSocketState>>;

} // namespace steamrot
