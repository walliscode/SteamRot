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
#include "Fragment.h"
#include "PartInstance.h"
#include "PartTraits.h"
#include "SocketState.h"

namespace steamrot {

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
};
} // namespace steamrot
