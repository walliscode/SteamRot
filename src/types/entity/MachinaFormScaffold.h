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
#include <cstdint>
#include <string>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class SocketState
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
/// @class JointInstance
/// @brief A placed instance of a Joint on the MachinaFormScaffold.
///
/// Holds a stable unique ID, a reference to the Joint definition, the single
/// transform that positions the instance on the canvas, and per-socket runtime
/// state. World positions for sockets are derived on demand via:
///   transform.transformPoint(joint.sockets[i])
/////////////////////////////////////////////////
struct JointInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a JointInstance from a Joint definition.
  ///
  /// Initialises socket_states with one default-constructed SocketState per
  /// socket in the Joint definition.
  ///
  /// @param joint_ref         Joint definition to reference.
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  JointInstance(Joint &joint_ref,
                sf::Transform initial_transform = sf::Transform::Identity)
      : joint{joint_ref}, transform{initial_transform} {
    socket_states.resize(joint_ref.sockets.size());
  }

  /////////////////////////////////////////////////
  /// @brief Stable ID assigned by MachinaFormScaffold when this instance is
  /// added. Unique across both joints and fragments.
  /////////////////////////////////////////////////
  uint32_t id{0};

  /////////////////////////////////////////////////
  /// @brief Joint definition being referenced.
  /////////////////////////////////////////////////
  Joint &joint;

  /////////////////////////////////////////////////
  /// @brief Single transform that positions this instance on the canvas.
  ///
  /// Apply to joint.sockets[i] to obtain each socket's world position.
  /////////////////////////////////////////////////
  sf::Transform transform{sf::Transform::Identity};

  /////////////////////////////////////////////////
  /// @brief Per-socket runtime state (connection status, hover).
  ///
  /// socket_states[i] corresponds to joint.sockets[i].
  /////////////////////////////////////////////////
  std::vector<SocketState> socket_states;
};

/////////////////////////////////////////////////
/// @class FragmentInstance
/// @brief A placed instance of a Fragment on the MachinaFormScaffold.
///
/// Holds a stable unique ID, a reference to the Fragment definition, the
/// single transform that positions the instance on the canvas, and per-socket
/// runtime state. World positions for sockets are derived on demand via:
///   transform.transformPoint(fragment.sockets[i])
/////////////////////////////////////////////////
struct FragmentInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a FragmentInstance from a Fragment definition.
  ///
  /// Initialises socket_states with one default-constructed SocketState per
  /// socket in the Fragment definition.
  ///
  /// @param fragment_ref      Fragment definition to reference.
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  FragmentInstance(
      Fragment &fragment_ref,
      sf::Transform initial_transform = sf::Transform::Identity)
      : fragment{fragment_ref}, transform{initial_transform} {
    socket_states.resize(fragment_ref.sockets.size());
  }

  /////////////////////////////////////////////////
  /// @brief Stable ID assigned by MachinaFormScaffold when this instance is
  /// added. Unique across both joints and fragments.
  /////////////////////////////////////////////////
  uint32_t id{0};

  /////////////////////////////////////////////////
  /// @brief Fragment definition being referenced.
  /////////////////////////////////////////////////
  Fragment &fragment;

  /////////////////////////////////////////////////
  /// @brief Single transform that positions this instance on the canvas.
  ///
  /// Apply to fragment.sockets[i] to obtain each socket's world position.
  /////////////////////////////////////////////////
  sf::Transform transform{sf::Transform::Identity};

  /////////////////////////////////////////////////
  /// @brief Per-socket runtime state (connection status, hover).
  ///
  /// socket_states[i] corresponds to fragment.sockets[i].
  /////////////////////////////////////////////////
  std::vector<SocketState> socket_states;
};

/////////////////////////////////////////////////
/// @class Connection
/// @brief Represents a connection between two sockets on the scaffold.
///
/// Each endpoint identifies a placed instance by its stable ID (not its
/// vector index, so connections survive reordering or deletion of other
/// parts) and the socket within that instance by index.
/////////////////////////////////////////////////
struct Connection {
  /////////////////////////////////////////////////
  /// @class Endpoint
  /// @brief One end of a Connection.
  /////////////////////////////////////////////////
  struct Endpoint {
    /////////////////////////////////////////////////
    /// @brief Stable instance ID (matches JointInstance::id or
    /// FragmentInstance::id).
    /////////////////////////////////////////////////
    uint32_t part_id{0};

    /////////////////////////////////////////////////
    /// @brief Index into that instance's socket_states (and the Part
    /// definition's sockets vector).
    /////////////////////////////////////////////////
    size_t socket_index{0};
  };

  Endpoint socket_a;
  Endpoint socket_b;
};

/////////////////////////////////////////////////
/// @class MachinaFormScaffold
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
  /// @brief All JointInstances placed on the scaffold.
  /////////////////////////////////////////////////
  std::vector<JointInstance> joints;

  /////////////////////////////////////////////////
  /// @brief All FragmentInstances placed on the scaffold.
  /////////////////////////////////////////////////
  std::vector<FragmentInstance> fragments;

  /////////////////////////////////////////////////
  /// @brief All connections between sockets on the scaffold.
  /////////////////////////////////////////////////
  std::vector<Connection> connections;
};

} // namespace steamrot
