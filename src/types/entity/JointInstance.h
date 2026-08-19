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

namespace steamrot {

enum class JointSocketPositioningStrategy {
  MaximizeDistance, ///< Maximize distance between sockets along the joint's
                    ///< length.
};

struct JointTraits {
  using SocketType = JointSocketState;

  using PartType = Joint;
};
class JointInstance : public PartInstance<JointTraits> {

public:
  /////////////////////////////////////////////////
  /// @brief Delete default constructor to prevent uninitialized instances.
  /////////////////////////////////////////////////
  JointInstance() = delete;

  /////////////////////////////////////////////////
  /// @brief Constructs a JointInstance with a given id and Joint reference.
  ///
  /// @param id Stable unique identifier for this instance.
  /// @param joint Reference to the Joint this instance represents.
  /////////////////////////////////////////////////
  JointInstance(const uint32_t id, const Joint &joint);

  /////////////////////////////////////////////////
  /// @brief Constructs a JointInstance with a given id, Joint reference, and
  /// alias.
  ///
  /// @param id Stable unique identifier for this instance.
  /// @param joint Reference to the Joint this instance represents.
  /// @param alias Human-readable identifier for this instance.
  /////////////////////////////////////////////////
  JointInstance(const uint32_t id, const Joint &joint,
                const std::string &alias);

  /////////////////////////////////////////////////
  /// @brief Positions the sockets of this JointInstance according to the
  /// specified strategy.
  ///
  /// @param positioning_strategy Enumeration value specifying the strategy to
  /// use for positioning the sockets.
  /////////////////////////////////////////////////
  void
  PositionSockets(const JointSocketPositioningStrategy positioning_strategy);

  sf::Vector2f GetSocketPivotWorldPosition() const {
    return GetTransform().transformPoint(part.socket_pivot);
  }
};

/////////////////////////////////////////////////
/// @brief Give a JointInstance, calculates the maximum possible even spread of
/// its sockets based on its SocketConfig and applies the resulting positions to
/// the instance's sockets.
///
/// @param instance JointInstance whose sockets should be maximally spread.
/////////////////////////////////////////////////
void maximise_joint_socket_spread(JointInstance::Sockets &joint_sockets,
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
    const JointInstance::Sockets &joint_sockets, const sf::Vector2f &pivot,
    const SocketConfig &config);

} // namespace steamrot
