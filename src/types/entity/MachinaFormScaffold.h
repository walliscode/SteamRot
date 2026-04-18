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
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdint>
#include <map>
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
struct SocketState {
  enum class State {
    Available, ///< Socket exists and can connect
    Connected, ///< Socket is currently connected
    Blocked    ///< Socket exists but can't be used (e.g., edge of canvas)
  };

  /////////////////////////////////////////////////
  /// @brief Connection state of this socket.
  /////////////////////////////////////////////////
  State state{State::Available};

  /////////////////////////////////////////////////
  /// @brief Whether the mouse is currently hovering over this socket.
  /////////////////////////////////////////////////
  bool is_mouse_over{false};
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
  SocketState state{};
};

/////////////////////////////////////////////////
/// @struct PartInstance
/// @brief Common base for all placed instances on the MachinaFormScaffold.
///
/// Holds the stable unique ID and the world-space transform shared by every
/// placed part. Socket data (position + state) is held in the concrete
/// subtype's @c sockets vector, which is fully initialised at construction.
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
/// All socket data (local position + runtime state) is stored in the @c sockets
/// vector, which is fully computed from the Joint's SocketConfig at construction
/// time. Positioning Logic may update individual @c sockets[i].local_position
/// entries at runtime (e.g. to rotate the socket ring). World-space position of
/// socket i is obtained via:
///   transform.transformPoint(sockets[i].local_position)
/////////////////////////////////////////////////
struct JointInstance : public PartInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a JointInstance from a Joint definition.
  ///
  /// Computes every socket's local-space position from the Joint's SocketConfig
  /// and stores it alongside a default-constructed SocketState in @c sockets.
  /// No further resizing or position assignment is needed after construction.
  ///
  /// @param joint_ptr         Pointer to the Joint definition. May be nullptr.
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  JointInstance(Joint *joint_ptr,
                sf::Transform initial_transform = sf::Transform::Identity)
      : PartInstance{initial_transform}, joint{joint_ptr} {
    if (!joint_ptr)
      return;

    const auto &config = joint_ptr->socket_config;
    if (config.socket_count == 0)
      return;

    const float arc_min = config.rotation_arc_min;
    const float arc_max = config.rotation_arc_max;
    const float arc_range = arc_max - arc_min;
    const float angle_between = arc_range / (config.socket_count + 1);

    sockets.reserve(config.socket_count);
    for (int i = 0; i < config.socket_count; ++i) {
      const float angle_deg = arc_min + angle_between * (i + 1);
      const float angle_rad = sf::degrees(angle_deg).asRadians();
      const sf::Vector2f pos =
          joint_ptr->origin +
          sf::Vector2f{std::cos(angle_rad), std::sin(angle_rad)} *
              config.radius;
      sockets.emplace_back(pos);
    }
  }

  /////////////////////////////////////////////////
  /// @brief Pointer to the Joint definition being referenced.
  /////////////////////////////////////////////////
  Joint *joint{nullptr};

  /////////////////////////////////////////////////
  /// @brief Per-socket data (local position + mutable state) for this Joint
  /// instance.
  ///
  /// Fully initialised from the Joint's SocketConfig at construction.
  /// Positioning Logic may update @c sockets[i].local_position to reposition
  /// individual sockets (e.g. to apply socket ring rotation). Apply the
  /// instance's transform to @c sockets[i].local_position to obtain the
  /// world-space position of socket @c i.
  /////////////////////////////////////////////////
  std::vector<SocketData> sockets;
};

/////////////////////////////////////////////////
/// @struct FragmentInstance
/// @brief A placed instance of a Fragment on the MachinaFormScaffold.
///
/// Derives from PartInstance and adds Fragment-specific runtime state.
/// All socket data (local position + runtime state) is stored in the @c sockets
/// vector, which is fully initialised at construction from the Fragment
/// definition's socket positions. World position of socket i is obtained via:
///   transform.transformPoint(sockets[i].local_position)
/////////////////////////////////////////////////
struct FragmentInstance : public PartInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a FragmentInstance from a Fragment definition.
  ///
  /// Populates @c sockets with one SocketData per socket in the Fragment
  /// definition, copying the local position and default-constructing the
  /// SocketState. No further resizing or position assignment is needed.
  ///
  /// @param fragment_ptr      Pointer to the Fragment definition. May be nullptr.
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  FragmentInstance(Fragment *fragment_ptr,
                   sf::Transform initial_transform = sf::Transform::Identity)
      : PartInstance{initial_transform}, fragment{fragment_ptr} {
    if (fragment_ptr) {
      sockets.reserve(fragment_ptr->sockets.size());
      for (const auto &pos : fragment_ptr->sockets) {
        sockets.emplace_back(pos);
      }
    }
  }

  /////////////////////////////////////////////////
  /// @brief Pointer to the Fragment definition being referenced.
  /////////////////////////////////////////////////
  Fragment *fragment{nullptr};

  /////////////////////////////////////////////////
  /// @brief Per-socket data (local position + mutable state) for this Fragment
  /// instance.
  ///
  /// Fully initialised from the Fragment definition at construction. Fragment
  /// socket positions are static (derived from the Part definition) and are not
  /// changed after construction. Apply the instance's transform to
  /// @c sockets[i].local_position to obtain the world-space position of socket
  /// @c i.
  /////////////////////////////////////////////////
  std::vector<SocketData> sockets;
};

/////////////////////////////////////////////////
/// @struct Connection
/// @brief Represents a connection between two sockets on the scaffold.
///
/// Each endpoint identifies a placed instance by its stable ID (not its
/// vector index, so connections survive reordering or deletion of other
/// parts) and the socket within that instance by index.
/////////////////////////////////////////////////
struct Connection {
  /////////////////////////////////////////////////
  /// @struct Endpoint
  /// @brief One end of a Connection.
  /////////////////////////////////////////////////
  struct Endpoint {
    /////////////////////////////////////////////////
    /// @brief Stable instance ID (matches JointInstance::id or
    /// FragmentInstance::id).
    /////////////////////////////////////////////////
    uint32_t part_id{0};

    /////////////////////////////////////////////////
    /// @brief Index into that instance's sockets vector.
    /////////////////////////////////////////////////
    size_t socket_index{0};
  };

  Endpoint socket_a;
  Endpoint socket_b;
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

  /////////////////////////////////////////////////
  /// @brief All connections between sockets on the scaffold.
  /////////////////////////////////////////////////
  std::vector<Connection> connections;

  // Boolean States //

  /////////////////////////////////////////////////
  /// @brief a boolean toggle used to determine whether to render sockets on the
  /// Joint and Fragments.
  /////////////////////////////////////////////////
  bool are_sockets_visible{false};
};

} // namespace steamrot
