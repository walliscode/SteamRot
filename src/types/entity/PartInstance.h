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
  /// @brief [TODO:description]
  ///
  /// @param angle [TODO:parameter]
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
  /// @brief Get a socket world alignment vector by rotating its local
  /// alignment vector by this instance total rotation.
  ///
  /// @param socket_id Socket identifier.
  /// @return World alignment vector, or (0, 0) if the socket does not exist.
  /////////////////////////////////////////////////
  sf::Vector2f GetSocketWorldAlignmentVector(uint32_t socket_id) const {
    const SocketType *socket = TryGetSocket(socket_id);
    if (!socket) {
      return sf::Vector2f(0.f, 0.f);
    }

    // get the socket's local alignment vector
    const sf::Vector2f &alignment_vec = socket->GetLocalAlignmentVector();

    // generate a transform from the total rotation of the part instance
    sf::Transform rotation_transform;
    rotation_transform.rotate(total_rotation);

    // apply the rotation transform to the local alignment vector to give the
    // world alignment vector and return
    return rotation_transform.transformPoint(alignment_vec);
  }

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

  bool CheckIfSocketIsAvailable(uint32_t socket_id) const {
    const SocketType *socket = TryGetSocket(socket_id);
    if (!socket) {
      return false;
    }
    return socket->GetConnectionState() == SocketConnectionState::Available;
  }
  std::optional<uint32_t> CheckIfAnySocketIsAvailable() const {
    for (const auto &[socket_id, socket] : sockets) {
      if (socket.GetConnectionState() == SocketConnectionState::Available) {
        return socket_id;
      }
    }
    return std::nullopt;
  }

  void ResetAllSocketsInteractionState() {
    for (auto &[socket_id, socket] : sockets) {
      socket.ResetInteractionState();
    }
  }
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
};

} // namespace steamrot
