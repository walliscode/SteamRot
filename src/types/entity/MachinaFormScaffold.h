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
#include "SocketConfigUtils.h"
#include <SFML/Graphics/Transform.hpp>
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
/// @struct PartInstance
/// @brief Common base for all placed instances on the MachinaFormScaffold.
///
/// Holds the stable unique ID, the world-space transform, and the per-socket
/// runtime state that are shared by every placed part. Derive from this struct
/// rather than duplicating these fields in JointInstance and FragmentInstance.
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

  /////////////////////////////////////////////////
  /// @brief Per-socket runtime state (connection status, hover).
  /////////////////////////////////////////////////
  std::vector<SocketState> socket_states;
};

/////////////////////////////////////////////////
/// @struct JointInstance
/// @brief A placed instance of a Joint on the MachinaFormScaffold.
///
/// Derives from PartInstance and adds Joint-specific runtime state.
/// Each socket's local-space position is stored in socket_local_positions and
/// initialised from the Joint's SocketConfig at construction time. Positioning
/// Logic may update individual entries to reposition sockets independently.
/// World-space position of socket i is obtained via:
///   transform.transformPoint(socket_local_positions[i])
/////////////////////////////////////////////////
struct JointInstance : public PartInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a JointInstance from a Joint definition.
  ///
  /// Initialises socket_states with one default-constructed SocketState per
  /// socket described by the Joint's SocketConfig, and populates
  /// socket_local_positions by computing each socket's initial local position
  /// from the SocketConfig at zero rotation.
  ///
  /// @param joint_ref         Joint definition to reference.
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  JointInstance(Joint &joint_ref,
                sf::Transform initial_transform = sf::Transform::Identity)
      : PartInstance{initial_transform}, joint{joint_ref} {
    const size_t count =
        static_cast<size_t>(joint_ref.socket_config.socket_count);
    socket_states.resize(count);
    socket_local_positions.reserve(count);
    for (size_t i = 0; i < count; ++i) {
      socket_local_positions.push_back(
          ComputeSocketLocalPos(joint_ref.socket_config, i, 0.f));
    }
  }

  /////////////////////////////////////////////////
  /// @brief Joint definition being referenced.
  /////////////////////////////////////////////////
  Joint &joint;

  /////////////////////////////////////////////////
  /// @brief Local-space positions of all sockets for this Joint instance.
  ///
  /// Initialised from the Joint's SocketConfig at construction. Positioning
  /// Logic may update individual entries to reposition sockets independently of
  /// one another. Apply the instance's transform to convert to world space.
  /////////////////////////////////////////////////
  std::vector<sf::Vector2f> socket_local_positions;
};

/////////////////////////////////////////////////
/// @struct FragmentInstance
/// @brief A placed instance of a Fragment on the MachinaFormScaffold.
///
/// Derives from PartInstance and adds Fragment-specific runtime state.
/// World positions for sockets are derived on demand via:
///   transform.transformPoint(fragment.sockets[i])
/////////////////////////////////////////////////
struct FragmentInstance : public PartInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a FragmentInstance from a Fragment definition.
  ///
  /// Initialises socket_states with one default-constructed SocketState per
  /// socket in the Fragment definition.
  ///
  /// @param fragment_ref      Fragment definition to reference.
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  FragmentInstance(Fragment &fragment_ref,
                   sf::Transform initial_transform = sf::Transform::Identity)
      : PartInstance{initial_transform}, fragment{fragment_ref} {
    socket_states.resize(fragment_ref.sockets.size());
  }

  /////////////////////////////////////////////////
  /// @brief Fragment definition being referenced.
  /////////////////////////////////////////////////
  Fragment &fragment;
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
    /// @brief Index into that instance's socket_states (and the Part
    /// definition's sockets vector).
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
