/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the JointInstance class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Joint.h"
#include "PartInstance.h"
#include "SocketState.h"
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <map>
#include <memory>

namespace steamrot {

using JointSockets = std::map<uint32_t, std::unique_ptr<JointSocketState>>;

enum class JointSocketPositioningStrategy {
  EvenSpread,       ///< Evenly spread sockets along the joint's length.
  MaximizeDistance, ///< Maximize distance between sockets along the joint's
                    ///< length.
};
/////////////////////////////////////////////////
/// @struct JointInstance
/// @brief An instance of a Joint with runtime state
/////////////////////////////////////////////////
class JointInstance : public PartInstance {

private:
  /////////////////////////////////////////////////
  /// @brief Pointer to the Joint definition being referenced.
  /////////////////////////////////////////////////
  const Joint *joint{nullptr};

  /////////////////////////////////////////////////
  /// @brief Map of socket indices to their runtime state.
  /////////////////////////////////////////////////
  JointSockets sockets;

  /////////////////////////////////////////////////
  /// @brief Pivot point of the joint in local space, used for socket
  /// positioning calculations.
  /////////////////////////////////////////////////
  const sf::Vector2f socket_pivot;

public:
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
  JointInstance(const Joint *joint_ptr,
                const JointSocketPositioningStrategy positioning_strategy =
                    JointSocketPositioningStrategy::EvenSpread,
                sf::Transform initial_transform = sf::Transform::Identity);

  /////////////////////////////////////////////////
  /// @brief [TODO:description]
  ///
  /// @param positioning_strategy [TODO:parameter]
  /////////////////////////////////////////////////
  void
  PositionSockets(const JointSocketPositioningStrategy positioning_strategy);

  /////////////////////////////////////////////////
  /// @brief [TODO:description]
  ///
  /// @param joint_socket_id [TODO:parameter]
  /////////////////////////////////////////////////
  sf::Vector2f
  CalculateAlignmentVectorWorld(const uint32_t joint_socket_id) const override;

  /////////////////////////////////////////////////
  /// @brief Getter for the Joint definition pointer.
  ///
  /// @return Pointer to the Joint definition being referenced.
  /////////////////////////////////////////////////
  const Joint *GetJoint() const { return joint; }

  /////////////////////////////////////////////////
  /// @brief Return the sockets for read only
  ///
  /// @return Map of socket indices to their runtime state.
  /////////////////////////////////////////////////

  const JointSocketState &GetSocket(const uint32_t socket_id) const override;

  JointSocketState &GetSocket(const uint32_t socket_id) override;

  /////////////////////////////////////////////////
  /// @brief [TODO:description]
  ///
  /// @param socket_id [TODO:parameter]
  /// @return [TODO:return]
  /////////////////////////////////////////////////
  const bool GetMouseOverSocketState(uint32_t socket_id) const;

  /////////////////////////////////////////////////
  /// @brief [TODO:description]
  ///
  /// @param world_mouse [TODO:parameter]
  /////////////////////////////////////////////////
  void CheckMouseOver(const sf::Vector2f &world_mouse) override;

  /////////////////////////////////////////////////
  /// @brief [TODO:description]
  ///
  /// @param connection [TODO:parameter]
  /// @param socket_id [TODO:parameter]
  /////////////////////////////////////////////////
  void SetConnection(const SocketConnection &connection,
                     const uint32_t socket_id) override;

  const sf::Vector2f
  GetSocketWorldPosition(const uint32_t socket_id) const override;

  const sf::Vector2f
  GetSocketWorldAlignmentVector(const uint32_t socket_id) const override;

  void ResetSocketState(const uint32_t socket_id);

  void ResetAllSocketStates();
};

/// FREE FUNCTIONS ///
/// These free functions are purely related to the JointInstance. They are
/// designed to be simple, functional and easily testable. They are then
/// intended to be cooridanted by the JointInstance class

/////////////////////////////////////////////////
/// @brief Give a JointInstance, calculates the maximum possible even spread of
/// its sockets based on its SocketConfig and applies the resulting positions to
/// the instance's sockets.
///
/// @param instance JointInstance whose sockets should be maximally spread.
/////////////////////////////////////////////////
void maximise_joint_socket_spread(JointSockets &joint_sockets,
                                  const sf::Vector2f &pivot,
                                  const SocketConfig &config);

/////////////////////////////////////////////////
/// @brief Checks if a JointInstance has a valid socket configuration based on
/// the sockets current positions
///
/// The sockets can move dynamically (locally which then transforms to global)
/// and this function checks if the current positions are valid based on the
/// JointInstance's SocketConfig.
/// @param joint_instance JointInstance to check
/// @return True if the socket configuration is valid, false otherwise
/////////////////////////////////////////////////
bool check_if_allowed_joint_socket_configuration(
    const JointSockets &joint_sockets, const sf::Vector2f &pivot,
    const SocketConfig &config);

} // namespace steamrot
