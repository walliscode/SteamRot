/////////////////////////////////////////////////////
/// @file
/// @brief Declaration of SocketState and socket connection types.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstdint>
#include <optional>

namespace steamrot {

static constexpr float k_proximity_distance_threshold = 10.f;
static constexpr float k_connection_distance_threshold = 2.5f;
/////////////////////////////////////////////////
/// @enum SocketConnectionState
/// @brief Runtime connection state of a socket.
/////////////////////////////////////////////////
enum class SocketConnectionState {
  Available, ///< Socket exists and can connect.
  Connected, ///< Socket is currently connected.
};

/////////////////////////////////////////////////
/// @struct SocketConnection
/// @brief Identifies the peer endpoint of an established socket connection.
/////////////////////////////////////////////////
struct SocketConnection {
  uint32_t peer_part_id{0};   ///< Stable ID of peer part instance.
  uint32_t peer_socket_id{0}; ///< Socket ID on peer instance.

  bool operator==(const SocketConnection &other) const {
    return peer_part_id == other.peer_part_id &&
           peer_socket_id == other.peer_socket_id;
  }
};

/////////////////////////////////////////////////
/// @struct JointFragmentConnection
/// @brief Convenience bundle for both ends of one joint-fragment connection.
/////////////////////////////////////////////////
struct JointFragmentConnection {
  /////////////////////////////////////////////////
  /// @brief Construct both ends of a joint-fragment connection pair.
  ///
  /// @param joint_id Stable ID of the joint instance.
  /// @param joint_socket_id Socket ID on the joint instance.
  /// @param fragment_id Stable ID of the fragment instance.
  /// @param fragment_socket_id Socket ID on the fragment instance.
  /////////////////////////////////////////////////
  explicit JointFragmentConnection(const uint32_t joint_id,
                                   const uint32_t joint_socket_id,
                                   const uint32_t fragment_id,
                                   const uint32_t fragment_socket_id)
      : joint_id{joint_id}, joint_socket_id{joint_socket_id},
        fragment_id{fragment_id}, fragment_socket_id{fragment_socket_id} {}

  const uint32_t joint_id;
  const uint32_t joint_socket_id;
  const uint32_t fragment_id;
  const uint32_t fragment_socket_id;
};

/////////////////////////////////////////////////
/// @struct SocketState
/// @brief Mutable runtime state shared by all socket state implementations.
/////////////////////////////////////////////////
struct SocketState {

protected:
  /////////////////////////////////////////////////
  /// @brief bool flag indicating whether the mouse is currently over this
  /// socket.
  /////////////////////////////////////////////////
  bool m_is_mouse_over{false};

  /////////////////////////////////////////////////
  /// @brief Optional float storing distance to nearest socket when proximity is
  /// active.
  /////////////////////////////////////////////////
  std::optional<float> m_distance_to_nearest_socket{std::nullopt};

  /////////////////////////////////////////////////
  /// @brief Optional SocketConnection storing the connected peer endpoint if
  /// connected.
  /////////////////////////////////////////////////
  std::optional<SocketConnection> m_connected_to{std::nullopt};

public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor for polymorphic base type.
  /////////////////////////////////////////////////
  virtual ~SocketState() = default;

  /////////////////////////////////////////////////
  /// @brief Returns whether the mouse is currently over this socket.
  ///
  /// @return True if mouse is over socket, false otherwise.
  /////////////////////////////////////////////////
  bool IsMouseOver() const { return m_is_mouse_over; }

  /////////////////////////////////////////////////
  /// @brief Returns whether another socket is currently within proximity range.
  ///
  /// @return True if another socket is near, false otherwise.
  /////////////////////////////////////////////////
  bool IsAnotherSocketNear() const {
    if (m_distance_to_nearest_socket.has_value()) {
      return m_distance_to_nearest_socket.value() <=
             k_proximity_distance_threshold;
    }
    return false;
  }

  /////////////////////////////////////////////////
  /// @brief Returns whether nearest candidate is within direct connection
  /// threshold.
  ///
  /// @return True if within connection threshold, false otherwise.
  /////////////////////////////////////////////////
  bool IsWithinConnectionDistance() const {
    if (!m_distance_to_nearest_socket.has_value())
      return false;
    return m_distance_to_nearest_socket.value() <=
           k_connection_distance_threshold;
  }

  /////////////////////////////////////////////////
  /// @brief Returns distance to nearest socket when proximity is active.
  ///
  /// @return Optional nearest distance in world units.
  /////////////////////////////////////////////////
  const std::optional<float> &GetDistanceToNearestSocket() const {
    return m_distance_to_nearest_socket;
  }

  /////////////////////////////////////////////////
  /// @brief Returns the connected peer endpoint if connected.
  ///
  /// @return Optional peer connection descriptor.
  /////////////////////////////////////////////////
  const std::optional<SocketConnection> &GetConnection() const {
    return m_connected_to;
  }

  /////////////////////////////////////////////////
  /// @brief Returns true when socket has no active connection endpoint.
  ///
  /// @return True if socket is available, false otherwise.
  /////////////////////////////////////////////////
  bool IsAvailable() const { return !m_connected_to.has_value(); }

  /////////////////////////////////////////////////
  /// @brief Returns connection state derived from @c connected_to.
  ///
  /// @return Connected when peer endpoint is present, otherwise Available.
  /////////////////////////////////////////////////
  SocketConnectionState GetConnectionState() const {
    return IsAvailable() ? SocketConnectionState::Available
                         : SocketConnectionState::Connected;
  }

  /////////////////////////////////////////////////
  /// @brief Returns whether this socket is ready to connect.
  ///
  /// @return True if ready to connect, false otherwise.
  /////////////////////////////////////////////////
  bool IsReadyToConnect() const { return IsAvailable(); }

  /////////////////////////////////////////////////
  /// @brief Set hover state of this socket.
  ///
  /// @param is_mouse_over New hover state.
  /////////////////////////////////////////////////
  void SetMouseOver(const bool is_mouse_over) {
    m_is_mouse_over = is_mouse_over;
  }

  /////////////////////////////////////////////////
  /// @brief Set optional nearest-socket distance value.
  ///
  /// @param distance Optional nearest distance.
  /////////////////////////////////////////////////
  void SetDistanceToNearestSocket(const std::optional<float> distance) {
    m_distance_to_nearest_socket = distance;
  }

  /////////////////////////////////////////////////
  /// @brief Record distance candidate if it improves current proximity state.
  ///
  /// Candidate is accepted only when:
  /// - it is within proximity threshold, and
  /// - there is no current candidate, or it is strictly closer.
  ///
  /// @param distance World-space distance to candidate peer socket.
  /// @return True when state updated with new candidate, false otherwise.
  /////////////////////////////////////////////////
  bool ConsiderCandidateDistance(const float distance) {
    if (distance > k_proximity_distance_threshold)
      return false;

    if (m_distance_to_nearest_socket.has_value() &&
        distance >= m_distance_to_nearest_socket.value()) {
      return false;
    }

    m_distance_to_nearest_socket = distance;
    return true;
  }

  /////////////////////////////////////////////////
  /// @brief Set or clear peer connection endpoint.
  ///
  /// @param connection Optional peer connection.
  /////////////////////////////////////////////////
  void SetConnection(const SocketConnection &connection) {
    m_connected_to = connection;
  }

  /////////////////////////////////////////////////
  /// @brief Clear connection endpoint and mark socket as unconnected.
  /////////////////////////////////////////////////
  void ClearConnection() { m_connected_to = std::nullopt; }

  /////////////////////////////////////////////////
  /// @brief Update hover state from world-space mouse and socket position.
  ///
  /// @param world_mouse Mouse position in world space.
  /// @param world_pos Socket position in world space.
  /// @param radius Hit radius used for overlap test.
  /////////////////////////////////////////////////
  void CheckMouseOver(const sf::Vector2f &world_mouse,
                      const sf::Vector2f &world_pos, const float radius = 5.f) {

    // Check if the mouse is within the hit radius of the socket position.
    m_is_mouse_over =
        (world_mouse - world_pos).x * (world_mouse - world_pos).x +
            (world_mouse - world_pos).y * (world_mouse - world_pos).y <=
        radius * radius;
  }

  /////////////////////////////////////////////////
  /// @brief Reset interaction-only fields.
  /////////////////////////////////////////////////
  void ResetInteractionState() {
    m_is_mouse_over = false;
    m_distance_to_nearest_socket = std::nullopt;
  }

  /////////////////////////////////////////////////
  /// @brief Reset all runtime fields, including connection state.
  /////////////////////////////////////////////////
  void ResetAllState() {
    ResetInteractionState();
    m_connected_to = std::nullopt;
  }

  /////////////////////////////////////////////////
  /// @brief Returns socket brightness in range [0, 255] from proximity
  /// distance.
  ///
  /// @return Brightness scalar in range [0, 255].
  /////////////////////////////////////////////////
  uint32_t GetSocketBrightness() const {
    static constexpr float range =
        k_proximity_distance_threshold - k_connection_distance_threshold;

    static_assert(
        range > 0.f,
        "proximity threshold must be strictly greater than connection "
        "threshold");
    float distance = m_distance_to_nearest_socket.value_or(
        k_proximity_distance_threshold + 1.f);

    const float t = (k_proximity_distance_threshold - distance) / range;
    const float clamped = std::clamp(t, 0.f, 1.f);
    return static_cast<uint8_t>(clamped * 255.f);
  }

  /////////////////////////////////////////////////
  /// @brief Returns socket-local position in owning part coordinates.
  ///
  /// @return Local position vector reference.
  /////////////////////////////////////////////////
  virtual const sf::Vector2f &GetLocalPosition() const = 0;

  /////////////////////////////////////////////////
  /// @brief Returns socket-local alignment vector in owning part coordinates.
  ///
  /// @return Local alignment vector reference.
  /////////////////////////////////////////////////
  virtual const sf::Vector2f &GetLocalAlignmentVector() const = 0;
};

/////////////////////////////////////////////////
/// @class TestSocketState
/// @brief Simple socket state for unit tests with fixed local geometry refs.
/////////////////////////////////////////////////
struct TestSocketState : public SocketState {
  explicit TestSocketState(const sf::Vector2f &local_pos,
                           const sf::Vector2f &alignment_vec)
      : m_local_position{local_pos}, m_alignment_vector{alignment_vec} {}

  const sf::Vector2f &GetLocalPosition() const override {
    return m_local_position;
  }

  const sf::Vector2f &GetLocalAlignmentVector() const override {
    return m_alignment_vector;
  }

private:
  const sf::Vector2f &m_local_position;
  const sf::Vector2f &m_alignment_vector;
};

/////////////////////////////////////////////////
/// @struct FragmentSocketState
/// @brief Socket state for fragment sockets with fixed local geometry refs.
/////////////////////////////////////////////////
struct FragmentSocketState : public SocketState {
  /////////////////////////////////////////////////
  /// @brief Construct fragment socket state from local geometry references.
  ///
  /// @param local_pos Local socket position reference from fragment
  /// definition.
  /// @param alignment_vec Local alignment vector reference from definition.
  /////////////////////////////////////////////////
  explicit FragmentSocketState(const sf::Vector2f &local_pos,
                               const sf::Vector2f &alignment_vec)
      : m_local_position{local_pos}, m_alignment_vector{alignment_vec} {}

  /////////////////////////////////////////////////
  /// @brief Returns fixed local position reference.
  ///
  /// @return Local position reference.
  /////////////////////////////////////////////////
  const sf::Vector2f &GetLocalPosition() const override {
    return m_local_position;
  }

  /////////////////////////////////////////////////
  /// @brief Returns fixed local alignment vector reference.
  ///
  /// @return Local alignment reference.
  /////////////////////////////////////////////////
  const sf::Vector2f &GetLocalAlignmentVector() const override {
    return m_alignment_vector;
  }

private:
  const sf::Vector2f &m_local_position;
  const sf::Vector2f &m_alignment_vector;
};

/////////////////////////////////////////////////
/// @struct JointSocketState
/// @brief Socket state for joints with mutable local geometry.
/////////////////////////////////////////////////
struct JointSocketState : public SocketState {
  /////////////////////////////////////////////////
  /// @brief Returns mutable local position value.
  ///
  /// @return Local position reference.
  /////////////////////////////////////////////////
  const sf::Vector2f &GetLocalPosition() const override {
    return m_local_position;
  }

  /////////////////////////////////////////////////
  /// @brief Returns local alignment vector value.
  ///
  /// @return Local alignment reference.
  /////////////////////////////////////////////////
  const sf::Vector2f &GetLocalAlignmentVector() const override {
    return m_alignment_vector;
  }

  /////////////////////////////////////////////////
  /// @brief Set local socket position.
  ///
  /// @param position New local position.
  /////////////////////////////////////////////////
  void SetLocalPosition(const sf::Vector2f &position) {
    m_local_position = position;
  }

  /////////////////////////////////////////////////
  /// @brief Set local alignment vector.
  ///
  /// @param alignment New local alignment vector.
  /////////////////////////////////////////////////
  void SetLocalAlignmentVector(const sf::Vector2f &alignment) {
    m_alignment_vector =
        alignment.normalized(); // Ensure alignment vector is normalized
  }

private:
  sf::Vector2f m_local_position{0.f, 0.f};
  sf::Vector2f m_alignment_vector{0.f, 0.f};
};

} // namespace steamrot
