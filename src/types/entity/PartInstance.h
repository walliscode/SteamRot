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
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Transformable.hpp>
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

template <typename Trait> class PartInstance : public sf::Transformable {

public:
  using SocketType = typename Trait::SocketType;
  using Sockets = std::map<uint32_t, SocketType>;
  using Part = typename Trait::PartType;
  using TraitType = Trait;

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
  /// @brief Stable unique identifier for this instance.
  /////////////////////////////////////////////////
  const uint32_t id;

  /////////////////////////////////////////////////
  /// @brief Human-readable identifier for this instance.
  /// Defaults to "none" if not provided.
  /////////////////////////////////////////////////
  const std::string alias{"none"};

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

    return getTransform().transformPoint(socket->GetLocalPosition());
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

      // get the distance between the mouse and the socket world position
      const sf::Vector2f diff = world_mouse - world_pos;

      // if the distance is less than the radius, set the socket's mouse-over
      // state to true
      socket.SetMouseOver(diff.length() <= 5.f);
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
    if (!socket) {

      return std::unexpected(FailInfo{
          FailMode::BadValue,
          std::format("socket_id {} does not exist on part instance {}",
                      socket_id, GetId())});
    }

    auto *other_socket = other_instance.TryGetSocket(other_socket_id);
    if (!other_socket) {

      return std::unexpected(FailInfo{
          FailMode::BadValue,
          std::format("socket_id {} does not exist on part instance {}",
                      other_socket_id, other_instance.GetId())});
    }

    // check for existing connection between the two instances
    if (!CheckForFirstConnectionWithOtherInstance(other_instance).has_value()) {

      return std::unexpected(FailInfo{
          FailMode::InvalidState,
          std::format("no connection exists between part instance {} and part "
                      "instance {}",
                      GetId(), other_instance.GetId())});
    }

    // ROTATION - THIS NEEDS TO GO FIRST //
    // attempt to get the world alignment vector of both sockets
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

    // calculate angle from this socket alignment vector to the other socket
    // alignment vector
    const sf::Angle rotation_angle = this_socket_world_alignment_vector.angleTo(
        other_socket_world_alignment_vector);

    // rotate this instance by the calculated angle around the pivot point of
    // this socket
    rotate(rotation_angle);

    // TRANSLATION //
    // move from this sockets world position to the other sockets world position
    move(other_instance.GetSocketWorldPosition(other_socket_id) -
         GetSocketWorldPosition(socket_id));

    return std::monostate{};
  }

  void DrawSockets(sf::RenderTexture &texture) const {

    static constexpr float k_outer_radius = 2.f;
    static constexpr float k_inner_radius = 1.f;
    static constexpr int k_point_count = 10;

    // Outer white circle drawn for every visible socket state.
    sf::CircleShape outer(k_outer_radius, k_point_count);
    outer.setOrigin({k_outer_radius, k_outer_radius});

    // draw an inner circle based on the socket state
    sf::CircleShape inner(k_inner_radius, k_point_count);
    inner.setOrigin({k_inner_radius, k_inner_radius});

    // cycle through all sockets and draw them based on their state
    for (const auto &[socket_id, socket_state] : sockets) {

      // transform the socket's local position to get its world position, then
      // use to draw the socket
      const sf::Vector2f world_pos = GetSocketWorldPosition(socket_id);
      outer.setPosition(world_pos);
      inner.setPosition(world_pos);

      // uses priority order to determine which state to draw, starting with the
      // most important state
      if (socket_state.IsWithinConnectionDistance()) {
        // Ready to connect: white outer + green inner circle.
        outer.setFillColor(sf::Color::White);
        texture.draw(outer);

        inner.setFillColor(sf::Color::Green);
        texture.draw(inner);

      } else if (socket_state.IsAnotherSocketNear()) {

        // Near but not ready: white outer + blue inner circle whose brightness
        // scales with proximity (proximity_scale 0 = dim, 255 = full blue).
        outer.setFillColor(sf::Color::White);
        texture.draw(outer);

        const uint8_t brightness = socket_state.GetSocketBrightness();
        inner.setFillColor(sf::Color{0, 0, brightness});
        texture.draw(inner);

      } else {
        // Default: white outer, or blue outer when the mouse hovers.
        outer.setFillColor(socket_state.IsMouseOver() ? sf::Color::Blue
                                                      : sf::Color::White);
        texture.draw(outer);
      }
    }
  }
};

} // namespace steamrot
