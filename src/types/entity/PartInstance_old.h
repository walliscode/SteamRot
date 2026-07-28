/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the PartInstance struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SocketState.h"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct PartInstance
/// @brief Common base for all placed instances on the MachinaFormScaffold.
///
/// Holds the stable unique ID, world-space transform, socket map, and
/// adjacency degree (connection_count) shared by every placed part.
/// Socket data (position + state) is fully initialised at construction
/// by the concrete subtype's constructor.
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
  /// @brief Human-readable alias for this instance (e.g. "f0", "j1").
  ///
  /// Optional. When populated, the analysis trace formatter will display this
  /// label instead of the raw numeric @c id. In production this is left empty;
  /// the test infrastructure (@c PartGraphBuilder) sets it to the user-friendly
  /// string alias passed to @c AddFragment / @c AddJoint.
  /////////////////////////////////////////////////
  std::string alias{};

  /////////////////////////////////////////////////
  /// @brief Single transform that positions this instance on the canvas.
  /////////////////////////////////////////////////
  sf::Transform transform{sf::Transform::Identity};

  /////////////////////////////////////////////////
  /// @brief Store the total rotation applied to this instance in degrees.
  /////////////////////////////////////////////////
  sf::Angle total_rotation{sf::degrees(0.f)};

  /////////////////////////////////////////////////
  /// @brief Number of established connections on this instance.
  ///
  /// Incremented by @c create_connection for each new connection added.
  /// Decremented by any future disconnect action. Mirrors adjacency degree
  /// directly on the scaffold so graph queries can skip the build step.
  /////////////////////////////////////////////////
  size_t connection_count{0};

  virtual sf::Vector2f
  CalculateAlignmentVectorWorld(const uint32_t socket_id) const = 0;

  virtual void SetConnection(const SocketConnection &connection,
                             const uint32_t socket_id) = 0;

  virtual void CheckMouseOver(const sf::Vector2f &world_mouse) = 0;

  virtual const sf::Vector2f
  GetSocketWorldPosition(const uint32_t socket_id) const = 0;

  virtual const sf::Vector2f
  GetSocketWorldAlignmentVector(const uint32_t socket_id) const = 0;

  virtual const SocketState &GetSocket(const uint32_t socket_id) const = 0;
  virtual SocketState &GetSocket(const uint32_t socket_id) = 0;
  virtual const std::map<uint32_t, std::unique_ptr<SocketState>> &
  GetSockets() const = 0;
  virtual std::map<uint32_t, std::unique_ptr<SocketState>> &GetSockets() = 0;
};
} // namespace steamrot
