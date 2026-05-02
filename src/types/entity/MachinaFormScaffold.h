/////////////////////////////////////////////////
/// @file
/// @brief Declarartion of the MachinFormScaffold struct. No implementation is
/// needed
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "Joint.h"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SocketState
/// @brief Runtime state of a single socket on a placed Fragment or Joint
/// instance.
///
/// Position is NOT stored here; it is always derived by applying the owning
/// instance's transform to the corresponding local socket position on the Part
/// definition. This keeps the transform as the single source of truth for all
/// positional elements.
/////////////////////////////////////////////////

enum class SocketState {
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
struct SocketData {
  /////////////////////////////////////////////////
  /// @brief Construct a SocketData with the given local-space position.
  ///
  /// @param pos Local-space position of this socket.
  /////////////////////////////////////////////////
  explicit SocketData(sf::Vector2f pos) : local_position{pos} {}

  /////////////////////////////////////////////////
  /// @brief Local-space position of this socket.
  ///
  /// Initialised at construction. Positioning Logic may update this value to
  /// reposition the socket (e.g. rotating a JointInstance's socket ring).
  /// Apply the owning instance's transform to obtain the world-space position.
  /////////////////////////////////////////////////
  sf::Vector2f local_position;

  /////////////////////////////////////////////////
  /// @brief Mutable runtime state of this socket (connection status, hover).
  /////////////////////////////////////////////////
  SocketState state{SocketState::Available};

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

/// using helpers ///
using SocketMap = std::map<uint32_t, SocketData>;

/////////////////////////////////////////////////
/// @struct PartInstance
/// @brief Common base for all placed instances on the MachinaFormScaffold.
///
/// Holds the stable unique ID and the world-space transform shared by every
/// placed part. Socket data (position + state) is held in the concrete
/// subtype's @c sockets map, which is fully initialised at construction.
/////////////////////////////////////////////////
struct PartInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a PartInstance with the given world-space transform.
  ///
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  explicit PartInstance(
      sf::Transform initial_transform = sf::Transform::Identity)
      : transform{initial_transform} {}

  /////////////////////////////////////////////////
  /// @brief Stable ID assigned by MachinaFormScaffold when this instance is
  /// added. Unique across both joints and fragments.
  /////////////////////////////////////////////////
  uint32_t id{0};

  /////////////////////////////////////////////////
  /// @brief Single transform that positions this instance on the canvas.
  /////////////////////////////////////////////////
  sf::Transform transform{sf::Transform::Identity};
};

/////////////////////////////////////////////////
/// @struct JointInstance
/// @brief A placed instance of a Joint on the MachinaFormScaffold.
///
/// Derives from PartInstance and adds Joint-specific runtime state.
/// The @c sockets map is sized to @c joint_ptr->socket_config.socket_count
/// at construction, with all positions zero-initialised. Call
/// @c initialize_joint_socket_positions() to populate the positions using the
/// even-spread algorithm before the instance is rendered or hit-tested.
/// World-space position of socket id is obtained via:
///   transform.transformPoint(sockets[id].local_position)
/////////////////////////////////////////////////
struct JointInstance : public PartInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a JointInstance from a Joint definition.
  ///
  /// Allocates one @c SocketData per socket declared in the Joint's
  /// SocketConfig (positions are zero-initialised). Call
  /// @c initialize_joint_socket_positions() afterwards to compute the
  /// even-spread positions from the Joint's SocketConfig.
  ///
  /// @param joint_ptr         Pointer to the Joint definition. May be nullptr.
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  JointInstance(Joint *joint_ptr,
                sf::Transform initial_transform = sf::Transform::Identity)
      : PartInstance{initial_transform}, joint{joint_ptr} {
    if (!joint_ptr)
      return;
    for (uint32_t i = 0; i < static_cast<uint32_t>(joint_ptr->socket_config.socket_count); ++i)
      sockets.emplace(i, SocketData{sf::Vector2f{0.f, 0.f}});
  }

  /////////////////////////////////////////////////
  /// @brief Pointer to the Joint definition being referenced.
  /////////////////////////////////////////////////
  Joint *joint{nullptr};

  /////////////////////////////////////////////////
  /// @brief Per-socket data (local position + mutable state) for this Joint
  /// instance, keyed by stable socket ID.
  ///
  /// Sized at construction; positions are zero-initialised. Call
  /// @c initialize_joint_socket_positions() to populate positions via the
  /// even-spread algorithm. Positioning Logic may subsequently update
  /// @c sockets[id].local_position to reposition individual sockets (e.g. to
  /// apply socket ring rotation). Apply the instance's transform to
  /// @c sockets[id].local_position to obtain the world-space position of socket
  /// @c id.
  /////////////////////////////////////////////////
  SocketMap sockets;
};

/////////////////////////////////////////////////
/// @struct FragmentInstance
/// @brief A placed instance of a Fragment on the MachinaFormScaffold.
///
/// Derives from PartInstance and adds Fragment-specific runtime state.
/// All socket data (local position + runtime state) is stored in the @c sockets
/// map, which is fully initialised at construction from the Fragment
/// definition's socket positions. World position of socket id is obtained via:
///   transform.transformPoint(sockets[id].local_position)
/////////////////////////////////////////////////
struct FragmentInstance : public PartInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a FragmentInstance from a Fragment definition.
  ///
  /// Populates @c sockets with one SocketData per socket in the Fragment
  /// definition, copying the local position and default-constructing the
  /// SocketState. No further resizing or position assignment is needed.
  ///
  /// @param fragment_ptr      Pointer to the Fragment definition. May be
  /// nullptr.
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  FragmentInstance(Fragment *fragment_ptr,
                   sf::Transform initial_transform = sf::Transform::Identity)
      : PartInstance{initial_transform}, fragment{fragment_ptr} {
    if (fragment_ptr) {
      for (uint32_t i = 0; i < static_cast<uint32_t>(fragment_ptr->sockets.size()); ++i)
        sockets.emplace(i, SocketData{fragment_ptr->sockets[i]});
    }
  }

  /////////////////////////////////////////////////
  /// @brief Pointer to the Fragment definition being referenced.
  /////////////////////////////////////////////////
  Fragment *fragment{nullptr};

  /////////////////////////////////////////////////
  /// @brief Per-socket data (local position + mutable state) for this Fragment
  /// instance, keyed by stable socket ID.
  ///
  /// Fully initialised from the Fragment definition at construction. Fragment
  /// socket positions are static (derived from the Part definition) and are not
  /// changed after construction. Apply the instance's transform to
  /// @c sockets[id].local_position to obtain the world-space position of socket
  /// @c id.
  /////////////////////////////////////////////////
  SocketMap sockets;
};

/// using helpers ///
using PartMap =
    std::map<uint32_t, std::variant<JointInstance, FragmentInstance>>;

/////////////////////////////////////////////////
/// 3struct MachinaFormScaffold
/// @brief Contains all data necessary to create a MachinaForm.
///
/// This is designed to be an "unbaked" editor workspace. When committed to a
/// MachinaForm, the scaffold data will be processed into efficient runtime
/// formats by the MachinaForm's own systems.
/////////////////////////////////////////////////
struct MachinaFormScaffold {

  /////////////////////////////////////////////////
  /// @brief Name transferred to the MachinaForm on commit.
  /////////////////////////////////////////////////
  std::string machina_form_name{""};

  /////////////////////////////////////////////////
  /// @brief Monotonically increasing counter used to assign stable IDs to
  /// newly added FragmentInstances and JointInstances.
  /////////////////////////////////////////////////
  uint32_t next_id{0};

  /////////////////////////////////////////////////
  /// @brief All placed parts (JointInstances and FragmentInstances) keyed by
  /// their stable IDs.
  ///
  /// Using std::map gives node-stable storage: insertions and erasures never
  /// invalidate references or iterators to other elements.
  /////////////////////////////////////////////////
  PartMap parts;

  // Boolean States //

  /////////////////////////////////////////////////
  /// @brief a boolean toggle used to determine whether to render sockets on the
  /// Joint and Fragments.
  /////////////////////////////////////////////////
  bool are_sockets_visible{false};
};

} // namespace steamrot
