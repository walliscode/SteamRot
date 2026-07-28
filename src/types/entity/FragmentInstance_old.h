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
#include "SocketState.h"
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <map>
namespace steamrot {

using FragmentSockets = std::map<uint32_t, FragmentSocketState>;

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
class FragmentInstance : public PartInstance {
private:
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
  FragmentSockets sockets;

public:
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
                   sf::Transform initial_transform = sf::Transform::Identity);

  sf::Vector2f CalculateAlignmentVectorWorld(
      const uint32_t fragment_socket_id) const override;

  const Fragment *GetFragment() const { return fragment; }

  const FragmentSocketState &GetSocket(const uint32_t socket_id) const;

  FragmentSocketState &GetSocket(const uint32_t socket_id);
  const FragmentSockets &GetSockets() const { return sockets; }

  void SetConnection(const SocketConnection &connection,
                     const uint32_t socket_id) override;

  void CheckMouseOver(const sf::Vector2f &world_mouse) override;

  const sf::Vector2f
  GetSocketWorldPosition(const uint32_t socket_id) const override;

  const sf::Vector2f
  GetSocketWorldAlignmentVector(const uint32_t socket_id) const override;
};
} // namespace steamrot
