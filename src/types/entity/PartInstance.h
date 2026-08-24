/////////////////////////////////////////////////
/// @file
/// @brief Templated PartInstance struct for concrete part types
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "PartTraits.h"
#include "SocketState.h"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdint>
#include <expected>
#include <format>
#include <map>
#include <string>
#include <variant>

namespace steamrot {

template <typename Trait> class PartInstance {

public:
  using SocketType = typename Trait::SocketType;
  using Sockets = std::map<uint32_t, SocketType>;
  using Part = typename Trait::PartType;

protected:
  template <typename> friend class PartInstance;
  /////////////////////////////////////////////////
  /// @brief Reference to the Part that this instance represents.
  /////////////////////////////////////////////////
  const Part &part;

  /////////////////////////////////////////////////
  /// @brief All sockets that belong to this instance, keyed by socket_id.
  /////////////////////////////////////////////////
  Sockets sockets;

  /////////////////////////////////////////////////
  /// @brief The global transform of this instance, including translation,
  /// rotation, and scale.
  /////////////////////////////////////////////////
  sf::Transform transform{sf::Transform::Identity};

  /////////////////////////////////////////////////
  /// @brief Stable unique identifier for this instance.
  /////////////////////////////////////////////////
  const uint32_t id;

  /////////////////////////////////////////////////
  /// @brief Human-readable identifier for this instance.
  /// Defaults to "none" if not provided.
  /////////////////////////////////////////////////
  const std::string alias{"none"};

  /////////////////////////////////////////////////
  /// @brief Total rotation applied to this instance in degrees.
  /////////////////////////////////////////////////
  sf::Angle total_rotation{sf::degrees(0.f)};

  /////////////////////////////////////////////////
  /// @brief Find a socket by id.
  ///
  /// @param socket_id Socket identifier.
  /// @return Pointer to the socket if found; nullptr otherwise.
  /////////////////////////////////////////////////
  SocketType *TryGetSocketMutable(uint32_t socket_id) noexcept {
    auto it = sockets.find(socket_id);
    return (it == sockets.end()) ? nullptr : &it->second;
  }

public:
  virtual ~PartInstance() = default;
  /////////////////////////////////////////////////
  /// @brief Construct a PartInstance with an id and referenced Part.
  ///
  /// @param id Stable unique identifier for this instance.
  /// @param part Reference to the Part this instance represents.
  /////////////////////////////////////////////////
  PartInstance(uint32_t id, const Part &part) : part(part), id(id) {}

  /////////////////////////////////////////////////
  /// @brief Construct a PartInstance with an id, alias, and referenced Part.
  ///
  /// @param id Stable unique identifier for this instance.
  /// @param alias Human-readable identifier for this instance.
  /// @param part Reference to the Part this instance represents.
  /////////////////////////////////////////////////
  PartInstance(uint32_t id, const std::string &alias, const Part &part)
      : part(part), id(id), alias(alias) {}

  /////////////////////////////////////////////////
  /// @brief Get mutable access to all sockets.
  ///
  /// @return Mutable reference to the socket map.
  /////////////////////////////////////////////////
  // Sockets &GetSockets() { return sockets; }

  /////////////////////////////////////////////////
  /// @brief Get read-only access to all sockets.
  ///
  /// @return Const reference to the socket map.
  /////////////////////////////////////////////////
  const Sockets &GetSockets() const { return sockets; }

  /////////////////////////////////////////////////
  /// @brief Return the number of sockets in this instance.
  ///
  /// @return size_t count of sockets.
  /////////////////////////////////////////////////
  uint32_t GetSocketCount() const { return sockets.size(); }

  /////////////////////////////////////////////////
  /// @brief Get the referenced Part.
  ///
  /// @return Const reference to the Part represented by this instance.
  /////////////////////////////////////////////////
  const Part &GetPart() const { return part; }

  /////////////////////////////////////////////////
  /// @brief Get this instance id.
  ///
  /// @return Stable unique identifier.
  /////////////////////////////////////////////////
  uint32_t GetId() const { return id; }

  /////////////////////////////////////////////////
  /// @brief Get this instance alias.
  ///
  /// @return Human-readable alias string.
  /////////////////////////////////////////////////
  const std::string &GetAlias() const { return alias; }

  /////////////////////////////////////////////////
  /// @brief Get mutable access to the global transform.
  ///
  /// @return Mutable reference to this instance transform.
  /////////////////////////////////////////////////
  sf::Transform &GetTransform() { return transform; }

  /////////////////////////////////////////////////
  /// @brief Get read-only access to the global transform.
  ///
  /// @return Const reference to this instance transform.
  /////////////////////////////////////////////////
  const sf::Transform &GetTransform() const { return transform; }

  /////////////////////////////////////////////////
  /// @brief Replace the global transform.
  ///
  /// @param new_transform New transform to apply.
  /////////////////////////////////////////////////
  void SetTransform(const sf::Transform &new_transform) {
    transform = new_transform;
  }

  /////////////////////////////////////////////////
  /// @brief Get read-only access to the total rotation of this instance.
  ///
  /// @return Const reference to the total rotation angle.
  /////////////////////////////////////////////////
  const sf::Angle &GetTotalRotation() const { return total_rotation; }

  /////////////////////////////////////////////////
  /// @brief Override the tracked total rotation of this instance.
  ///
  /// @param angle sf::Angle to set as the new total rotation.
  /////////////////////////////////////////////////
  void SetTotalRotation(const sf::Angle &angle) { total_rotation = angle; }

  /////////////////////////////////////////////////
  /// @brief Add an angle to the tracked total rotation.
  ///
  /// @param angle Angle to accumulate.
  /////////////////////////////////////////////////
  void AddToTotalRotation(const sf::Angle &angle) { total_rotation += angle; }

  /////////////////////////////////////////////////
  /// @brief Find a socket by id (const overload).
  ///
  /// @param socket_id Socket identifier.
  /// @return Const pointer to the socket if found; nullptr otherwise.
  /////////////////////////////////////////////////
  const SocketType *TryGetSocket(uint32_t socket_id) const noexcept {
    auto it = sockets.find(socket_id);
    return (it == sockets.end()) ? nullptr : &it->second;
  }

  /////////////////////////////////////////////////
  /// @brief Get number of connected sockets in this instance.
  ///
  /// @return number of sockets with connection state Connected.
  /////////////////////////////////////////////////
  uint32_t GetNumberOfConnectedSockets() const {
    uint32_t count = 0;
    for (const auto &[socket_id, socket] : sockets) {
      if (socket.GetConnectionState() == SocketConnectionState::Connected) {
        count++;
      }
    }
    return count;
  }

  /////////////////////////////////////////////////
  /// @brief Return the local position of a socket by its id, or (0, 0) if the
  /// socket does not exist.
  ///
  /// @param socket_id  Socket identifier.
  /// @return sf::Vector2f Local position of the socket, or (0, 0) if the socket
  /// does not exist.
  /////////////////////////////////////////////////
  const sf::Vector2f GetSocketLocalPosition(uint32_t socket_id) const {
    const SocketType *socket = TryGetSocket(socket_id);
    if (!socket) {
      return sf::Vector2f(0.f, 0.f);
    }
    return socket->GetLocalPosition();
  }
  /////////////////////////////////////////////////
  /// @brief Get a socket world position by transforming its local position.
  ///
  /// @param socket_id Socket identifier.
  /// @return World position of the socket, or (0, 0) if the socket does not
  /// exist.
  /////////////////////////////////////////////////
  sf::Vector2f GetSocketWorldPosition(uint32_t socket_id) const {
    const SocketType *socket = TryGetSocket(socket_id);
    if (!socket) {
      return sf::Vector2f(0.f, 0.f);
    }

    return transform.transformPoint(socket->GetLocalPosition());
  }

  const sf::Vector2f GetSocketLocalAlignmentVector(uint32_t socket_id) const {
    const SocketType *socket = TryGetSocket(socket_id);
    if (!socket) {
      return sf::Vector2f(0.f, 0.f);
    }
    return socket->GetLocalAlignmentVector();
  }

  /////////////////////////////////////////////////
  /// @brief Get a socket world alignment vector
  ///
  /// This is currently Socket specific and must be implemented in the derived
  /// class.
  /// @param socket_id Socket identifier.
  /////////////////////////////////////////////////
  virtual std::expected<sf::Vector2f, FailInfo>
  GetSocketWorldAlignmentVector(uint32_t socket_id) const = 0;

  /////////////////////////////////////////////////
  /// @brief Update mouse-over state for all sockets using current world-space
  /// socket positions.
  ///
  /// @param world_mouse Mouse position in world space.
  /////////////////////////////////////////////////
  void CheckMouseOverSockets(const sf::Vector2f &world_mouse) {
    for (auto &[socket_id, socket] : sockets) {
      const sf::Vector2f world_pos = GetSocketWorldPosition(socket_id);
      socket.CheckMouseOver(world_mouse, world_pos);
    }
  }

  /////////////////////////////////////////////////
  /// @brief Check a specific socket's availability for connection. returns
  /// false if the socket does not exist or is not available.
  ///
  /// @param socket_id Stable unique identifier for the socket to check.
  /// @return True if the socket exists and is available for connection; false
  /// otherwise.
  /////////////////////////////////////////////////
  bool CheckIfSocketIsAvailable(uint32_t socket_id) const {
    const SocketType *socket = TryGetSocket(socket_id);
    if (!socket) {
      return false;
    }
    return socket->GetConnectionState() == SocketConnectionState::Available;
  }
  /////////////////////////////////////////////////
  /// @brief Check if any socket in this instance is available for connection.
  ///
  /// @return an optional containing the first available socket id, or
  /// std::nullopt if none are available.
  /////////////////////////////////////////////////
  std::optional<uint32_t> CheckIfAnySocketIsAvailable() const {
    for (const auto &[socket_id, socket] : sockets) {
      if (socket.GetConnectionState() == SocketConnectionState::Available) {
        return socket_id;
      }
    }
    return std::nullopt;
  }

  /////////////////////////////////////////////////
  /// @brief resets all interaction state for all sockets
  /////////////////////////////////////////////////
  void ResetAllSocketsInteractionState() {
    for (auto &[socket_id, socket] : sockets) {
      socket.ResetInteractionState();
    }
  }

  /////////////////////////////////////////////////
  /// @brief Resets all state for all sockets
  /////////////////////////////////////////////////
  void ResetAllSocketState() {
    for (auto &[socket_id, socket] : sockets) {
      socket.ResetAllState();
    }
  }

  /////////////////////////////////////////////////
  /// @brief Check for collisions between this PartInstance's sockets and
  /// another
  ///
  /// By using a template parameter for the other PartInstance type, this
  /// function can be used to check for collisions between any two PartInstance
  /// types, such as FragmentInstance and JointInstance. Coupled with the friend
  /// class statement, allows derivates of PartInstance to access the private
  /// members of other PartInstance types, enabling collision checks between
  /// different part types.
  /// @tparam OtherTrait Template parameter for the other PartInstance type.
  /// @param other_instance other PartInstance to check for collisions against.
  /////////////////////////////////////////////////
  template <typename OtherTrait>
    requires CompatibleTraits<Trait, OtherTrait>
  void
  CheckWithOtherInstanceForCollision(PartInstance<OtherTrait> &other_instance) {

    for (auto &[socket_id, socket] : sockets) {
      for (auto &[other_id, other_socket] : other_instance.sockets) {
        if (!socket.IsAvailable() || !other_socket.IsAvailable())
          continue;

        const sf::Vector2f p1 = GetSocketWorldPosition(socket_id);
        const sf::Vector2f p2 = other_instance.GetSocketWorldPosition(other_id);

        const float distance = std::hypot(p1.x - p2.x, p1.y - p2.y);

        socket.ConsiderCandidateDistance(distance);
        other_socket.ConsiderCandidateDistance(distance);
      }
    }
  }

  /////////////////////////////////////////////////
  /// @brief Creates a connection between this PartInstance and another
  /// PartInstance, given the socket IDs to connect.
  ///
  /// @tparam OtherTrait The trait type of the other PartInstance, which must be
  /// compatible with this PartInstance's trait. Compatibility can be found in
  /// PartTraits.h.
  /// @param socket_id Stable unique identifier for the socket in this
  /// PartInstance to connect.
  /// @param other_instance PartInstance of the other type to connect with.
  /// @param other_socket_id Stable unique identifier for the socket in the
  /// other PartInstance to connect.
  /// @return std::monostate on success, or std::unexpected containing FailInfo
  /// on failure.
  /////////////////////////////////////////////////
  template <typename OtherTrait>
    requires CompatibleTraits<Trait, OtherTrait>
  std::expected<std::monostate, FailInfo>
  CreateConnectionWithOtherInstance(uint32_t socket_id,
                                    PartInstance<OtherTrait> &other_instance,
                                    uint32_t other_socket_id) {

    // check sockets exist on both instances
    SocketType *socket = TryGetSocketMutable(socket_id);
    if (!socket)
      return std::unexpected(FailInfo{
          FailMode::MissingData, "Socket with ID " + std::to_string(socket_id) +
                                     " does not exist in this PartInstance."});

    auto *other_socket = other_instance.TryGetSocketMutable(other_socket_id);
    if (!other_socket)
      return std::unexpected(
          FailInfo{FailMode::MissingData,
                   "Socket with ID " + std::to_string(other_socket_id) +
                       " does not exist in the other PartInstance."});

    // all checks necessary for connection creation:
    if (!socket->IsAvailable() || !other_socket->IsAvailable())
      return std::unexpected(
          FailInfo{FailMode::InvalidState,
                   "One of the sockets is not available for connection."});

    // create the connection objects for both sockets
    SocketConnection connection{
        .peer_part_id = other_instance.GetId(),
        .peer_socket_id = other_socket_id,
    };
    socket->SetConnection(connection);
    SocketConnection other_connection{
        .peer_part_id = GetId(),
        .peer_socket_id = socket_id,
    };
    other_socket->SetConnection(other_connection);

    return std::monostate{};
  }

  /////////////////////////////////////////////////
  /// @brief Checks for socket connections between this PartInstance and another
  /// PartInstance.
  ///
  /// @tparam OtherTrait The type of the other PartInstance. Currently not
  /// limited to any specific PartInstance type.
  /// @param other_instance PartInstance of the other type to check for
  /// connections with.
  /// @return
  /////////////////////////////////////////////////
  template <typename OtherTrait>
  std::optional<PartToPartConnection> CheckForFirstConnectionWithOtherInstance(
      const PartInstance<OtherTrait> &other_instance) const {

    const auto this_id = GetId();
    const auto other_id = other_instance.GetId();

    // A PartInstance cannot be connected to itself.
    if (this_id == other_id) {
      return std::nullopt;
    }

    // Iterate through all sockets on this instance looking for a connection
    // whose peer is the other instance.
    for (const auto &[this_socket_id, this_socket] : sockets) {
      // Skip sockets that are not marked connected.
      if (this_socket.GetConnectionState() !=
          SocketConnectionState::Connected) {
        continue;
      }

      // Get this socket's connection payload (optional for safety/integrity).
      const auto this_conn = this_socket.GetConnection();
      if (!this_conn) {
        continue; // State says "connected" but no payload; treat as non-match.
      }

      // Only consider connections that point to the queried other instance.
      if (this_conn->peer_part_id != other_id) {
        continue;
      }

      // The referenced peer socket must exist on the other instance.
      const auto *other_socket =
          other_instance.TryGetSocket(this_conn->peer_socket_id);
      if (!other_socket) {
        continue;
      }

      // Peer socket must also be marked connected.
      if (other_socket->GetConnectionState() !=
          SocketConnectionState::Connected) {
        continue;
      }

      // Peer socket must have a reciprocal connection payload.
      const auto other_conn = other_socket->GetConnection();
      if (!other_conn) {
        continue;
      }

      // Reciprocal integrity check:
      // other_socket must point back to (this_id, this_socket_id).
      if (other_conn->peer_part_id == this_id &&
          other_conn->peer_socket_id == this_socket_id) {
        // First valid, existing part-to-part connection found.
        return PartToPartConnection{
            this_id,
            this_socket_id,
            other_id,
            this_conn->peer_socket_id,
        };
      }
    }

    // No valid reciprocal connection found between these two instances.
    return std::nullopt;
  }

  template <typename OtherTrait>
    requires CompatibleTraits<Trait, OtherTrait>
  std::expected<std::monostate, FailInfo>
  AlignOntoOtherPartInstance(const uint32_t socket_id,
                             const PartInstance<OtherTrait> &other_instance,
                             const uint32_t other_socket_id) {
    // check sockets exist on both instances
    SocketType *socket = TryGetSocketMutable(socket_id);
    if (!socket)
      return std::unexpected(FailInfo{
          FailMode::BadValue,
          std::format("socket_id {} does not exist on part instance {}",
                      socket_id, GetId())});

    auto *other_socket = other_instance.TryGetSocket(other_socket_id);
    if (!other_socket)
      return std::unexpected(FailInfo{
          FailMode::BadValue,
          std::format("socket_id {} does not exist on part instance {}",
                      other_socket_id, other_instance.GetId())});

    // check for existing connection between the two instances
    if (!CheckForFirstConnectionWithOtherInstance(other_instance).has_value()) {
      return std::unexpected(FailInfo{
          FailMode::InvalidState,
          std::format("no connection exists between part instance {} and part "
                      "instance {}",
                      GetId(), other_instance.GetId())});
    }

    // attempt toget the world alignment vector of both sockets
    // return an error if either socket's world alignment vector cannot be
    // retrieved
    const auto this_socket_world_alignment_vector_result =
        GetSocketWorldAlignmentVector(socket_id);
    if (!this_socket_world_alignment_vector_result.has_value()) {
      return std::unexpected(FailInfo{
          FailMode::InvalidState,
          std::format(
              "failed to get world alignment vector for socket_id {} on "
              "part instance {}",
              socket_id, GetId())});
    }
    const auto other_socket_world_alignment_vector_result =
        other_instance.GetSocketWorldAlignmentVector(other_socket_id);
    if (!other_socket_world_alignment_vector_result.has_value()) {
      return std::unexpected(FailInfo{
          FailMode::InvalidState,
          std::format(
              "failed to get world alignment vector for socket_id {} on "
              "part instance {}",
              other_socket_id, other_instance.GetId())});
    }

    // pull out the values
    const sf::Vector2f this_socket_world_alignment_vector =
        this_socket_world_alignment_vector_result.value();
    const sf::Vector2f other_socket_world_alignment_vector =
        other_socket_world_alignment_vector_result.value();

    // calculate cross and dot for atan2
    float cross_val = this_socket_world_alignment_vector.cross(
        other_socket_world_alignment_vector);
    float dot_val = this_socket_world_alignment_vector.dot(
        other_socket_world_alignment_vector);

    // angle in radians from source -> target
    float angle = std::atan2(cross_val, dot_val);
    const sf::Angle rotation_angle = sf::radians(angle);

    // create a rotation transform from this angle
    sf::Transform rotation_transform{sf::Transform::Identity};
    rotation_transform.rotate(rotation_angle);

    // rotate the fragment socket world position by the rotation transform
    const sf::Vector2f rotated_this_socket_world_position =
        rotation_transform.transformPoint(GetSocketWorldPosition(socket_id));

    // calculate the translation vector to align the rotated socket with the
    // other socket
    const sf::Vector2f translation_vector =
        other_instance.GetSocketWorldPosition(other_socket_id) -
        rotated_this_socket_world_position;

    // BUILD THE FINAL TRANSFORM //
    // reset the transform to identity
    transform = sf::Transform::Identity;
    // translate the rotated socket to the other socket's world position
    transform.translate(translation_vector);
    // rotate the instance to align the socket alignment vectors
    transform.rotate(rotation_angle);

    // update the total rotation of this instance
    total_rotation = rotation_angle;

    return std::monostate{};
  }
};

} // namespace steamrot
