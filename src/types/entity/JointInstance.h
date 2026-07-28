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

namespace steamrot {

enum class JointSocketPositioningStrategy {
  EvenSpread,       ///< Evenly spread sockets along the joint's length.
  MaximizeDistance, ///< Maximize distance between sockets along the joint's
                    ///< length.
};

struct JointTraits {
  using SocketType = JointSocketState;

  using PartType = Joint;
};
class JointInstance : public PartInstance<JointTraits> {

public:
  JointInstance(const uint32_t id, const Joint &joint)
      : PartInstance<JointTraits>(id, joint) {}

  JointInstance(const uint32_t id, const Joint &joint, const std::string &alias)
      : PartInstance<JointTraits>(id, alias, joint) {}

  void
  PositionSockets(const JointSocketPositioningStrategy positioning_strategy);
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
