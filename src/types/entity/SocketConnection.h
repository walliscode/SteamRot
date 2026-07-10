/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SocketConnection struct.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "SocketState.h"
#include <memory>

namespace steamrot {

struct SocketConnection : std::enable_shared_from_this<SocketConnection> {

public:
  FragmentSocketState &GetFragmentSocketState() const {
    return *fragment_socket_connection.lock();
  }
  JointSocketState &GetJointSocketState() const {
    return *joint_socket_connection.lock();
  }

  static std::shared_ptr<SocketConnection>
  Create(std::shared_ptr<JointSocketState> joint_socket,
         std::shared_ptr<FragmentSocketState> fragment_socket) {

    // Make a shared_ptr and thus the object of SocketConnection
    auto connection = std::make_shared<SocketConnection>();

    // Set the weak_ptrs to the provided socket states
    connection->joint_socket_connection = joint_socket;
    connection->fragment_socket_connection = fragment_socket;

    // Set the connection pointers in the socket states to this connection
    if (joint_socket) {
      joint_socket->connection =
    }
    return connection;
  }

private:
  std::weak_ptr<JointSocketState> joint_socket_connection;

  std::weak_ptr<FragmentSocketState> fragment_socket_connection;
};

} // namespace steamrot
