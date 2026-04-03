/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the GhostItemState struct.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <optional>
#include <string>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct GhostItemState
/// @brief Holds the runtime state of a single "ghost" (preview) item being
/// dragged and placed by the player.
///
/// Only one ghost item may be active per scene at any time. The state is
/// owned by SceneResources and accessed via SceneContext, so it lives
/// outside the ECS and does not occupy an entity slot.
///
/// Lifecycle:
///  - A ghost is activated by firing a LogicPayload with
///    LogicToggle::SELECT_GHOST_ITEM (carrying the item key).
///  - The ghost follows the mouse each tick while active.
///  - A left-click fires LogicToggle::PLACE_GHOST_ITEM and clears the state.
///  - A cancel signal fires LogicToggle::CLEAR_GHOST_ITEM, also clearing the
///    state.
/////////////////////////////////////////////////
struct GhostItemState {

  /////////////////////////////////////////////////
  /// @brief Optional string key identifying the item currently held.
  ///
  /// The key corresponds to an entry in the relevant asset collection
  /// (e.g. GrimoireMachina::m_all_fragments, m_all_joints, etc.).
  /// nullopt when no ghost item is active.
  /////////////////////////////////////////////////
  std::optional<std::string> m_item_key{};

  /////////////////////////////////////////////////
  /// @brief Current screen-space position of the ghost, updated each tick
  /// to follow the mouse when active.
  /////////////////////////////////////////////////
  sf::Vector2f m_position{0.f, 0.f};

  /////////////////////////////////////////////////
  /// @brief Whether a ghost item is currently being held / previewed.
  /////////////////////////////////////////////////
  bool m_is_active{false};
};

} // namespace steamrot
