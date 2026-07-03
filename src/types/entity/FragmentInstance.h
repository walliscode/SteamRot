/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the FragmentInstance struct.
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
#include "SocketState.h"
#include <cstdint>
#include <map>
namespace steamrot {
/////////////////////////////////////////////////
/// @struct FragmentInstance
/// @brief A placed instance of a Fragment on the MachinaFormScaffold.
///
/// Derives from PartInstance and adds Fragment-specific runtime state.
/// All socket data (local position + runtime state) is stored in the @c sockets
/// map, which is fully initialised at construction from the Fragment
/// definition's socket positions. World position of socket id is obtained via:
///   transform.transformPoint(sockets[id].local_position)
/////////////////////////////////////////////////
struct FragmentInstance : public PartInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a FragmentInstance from a Fragment definition.
  ///
  /// Populates @c sockets with one SocketData per socket in the Fragment
  /// definition, copying the local position and default-constructing the
  /// SocketState. No further resizing or position assignment is needed.
  ///
  /// @param fragment_ptr      Pointer to the Fragment definition. May be
  /// nullptr.
  /// @param initial_transform Transform placing this instance in world space.
  /////////////////////////////////////////////////
  FragmentInstance(const Fragment *fragment_ptr,
                   sf::Transform initial_transform = sf::Transform::Identity)
      : PartInstance{initial_transform}, fragment{fragment_ptr} {
    if (fragment_ptr) {
      for (uint32_t i = 0;
           i < static_cast<uint32_t>(fragment_ptr->sockets.size()); ++i)
        sockets.emplace(
            i, FragmentSocketState{fragment_ptr->sockets[i].local_position});
    }
  }

  /////////////////////////////////////////////////
  /// @brief Pointer to the Fragment definition being referenced.
  /////////////////////////////////////////////////
  const Fragment *fragment{nullptr};

  /////////////////////////////////////////////////
  /// @brief Map of socket IDs to their local positions and mutable runtime
  /// state.
  ///
  /// N.B. The socket references fixed positions from the Fragment definition,
  /// these will need transforming by the FragmentInstance's transform
  /////////////////////////////////////////////////
  std::map<uint32_t, FragmentSocketState> sockets;
};
} // namespace steamrot
