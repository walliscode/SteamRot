/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the JointInstance struct.
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
#include <map>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct JointInstance
/// @brief An instance of a Joint with runtime state
/////////////////////////////////////////////////
struct JointInstance : public PartInstance {
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
                sf::Transform initial_transform = sf::Transform::Identity)
      : PartInstance{initial_transform}, joint{joint_ptr} {
    if (!joint_ptr)
      return;
    for (uint32_t i = 0;
         i < static_cast<uint32_t>(joint_ptr->socket_config.socket_count); ++i)
      sockets.emplace(i, JointSocketState{});
  }

  /////////////////////////////////////////////////
  /// @brief Pointer to the Joint definition being referenced.
  /////////////////////////////////////////////////
  const Joint *joint{nullptr};

  std::map<uint32_t, JointSocketState> sockets{};
};
} // namespace steamrot
