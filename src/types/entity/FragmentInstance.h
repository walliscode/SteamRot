/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the FragmentInstance class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "Fragment.h"
#include "PartInstance.h"
#include "PartTraits.h"
#include "SocketState.h"
#include <SFML/System/Vector2.hpp>
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class FragmentInstance
/// @brief A instance of a Fragment part, with its own state and socket
/// configuration.
/////////////////////////////////////////////////
class FragmentInstance : public PartInstance<FragmentTraits> {

public:
  /////////////////////////////////////////////////
  /// @brief Delete default constructor to prevent instances without socket
  /// configuration.
  /////////////////////////////////////////////////
  FragmentInstance() = delete;

  /////////////////////////////////////////////////
  /// @brief Constructs a FragmentInstance with a given id and Fragment
  /// reference.
  ///
  /// @param id Stable unique identifier for this instance.
  /// @param fragment Fragment reference to the Fragment this instance
  /// represents.
  /////////////////////////////////////////////////
  FragmentInstance(const uint32_t id, const Fragment &fragment)
      : PartInstance<FragmentTraits>(id, fragment) {

    // initialise the sockets based on the Fragment
    for (uint32_t i = 0; i < fragment.sockets.size(); ++i) {
      const auto &socket_data = fragment.sockets[i];
      sockets.emplace(i, FragmentSocketState{socket_data.local_position,
                                             socket_data.alignment_vector});
    }
  }

  /////////////////////////////////////////////////
  /// @brief Constructs a FragmentInstance with a given id, Fragment reference,
  /// and alias.
  ///
  /// @param id Stable unique identifier for this instance.
  /// @param fragment Fragment reference to the Fragment this instance
  /// represents.
  /// @param alias Human-readable identifier for this instance
  /////////////////////////////////////////////////
  FragmentInstance(const uint32_t id, const Fragment &fragment,
                   const std::string &alias)
      : PartInstance<FragmentTraits>(id, alias, fragment) {
    // initialise the sockets based on the Fragment
    for (uint32_t i = 0; i < fragment.sockets.size(); ++i) {
      const auto &socket_data = fragment.sockets[i];
      sockets.emplace(i, FragmentSocketState{socket_data.local_position,
                                             socket_data.alignment_vector});
    }
  }

  /////////////////////////////////////////////////
  /// @brief Get a socket world alignment vector
  ///
  /// @param socket_id Socket identifier.
  /// @return Alignment vector of the socket in world coordinates, or an error
  /// if the socket does not exist.
  /////////////////////////////////////////////////
  std::expected<sf::Vector2f, FailInfo>
  GetSocketWorldAlignmentVector(uint32_t socket_id) const override {
    const SocketType *socket = TryGetSocket(socket_id);
    if (!socket) {
      return std::unexpected(
          FailInfo{FailMode::MissingData,
                   "Socket with ID " + std::to_string(socket_id) +
                       " does not exist in this FragmentInstance."});
    }
    // pull out the total rotation of the fragment instance and apply it to the
    // socket's local alignment vector. Nomalize the result to ensure it is a
    // unit vector.
    return socket->GetLocalAlignmentVector()
        .rotatedBy(getRotation())
        .normalized();
  }
};
} // namespace steamrot
