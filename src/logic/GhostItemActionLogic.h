/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the GhostItemActionLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Logic.h"
#include "LogicType.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
/// @class GhostItemActionLogic
/// @brief Handles the "ghost item" placement workflow.
///
/// Responsibilities:
///  - Activates / deactivates the GhostItemState in SceneContext in response
///    to SELECT_GHOST_ITEM and CLEAR_GHOST_ITEM logic events.
///  - Updates the ghost position to follow the mouse every tick while active.
///  - On a SELECT input (left click), places the ghost item by firing a
///    PLACE_GHOST_ITEM logic event carrying the item key and then clears the
///    ghost state.
///
/// This Logic is scene-agnostic: it can be added to any scene that needs
/// item placement, and the PLACE_GHOST_ITEM event can be handled by a
/// domain-specific Logic class.
/////////////////////////////////////////////////
class GhostItemActionLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Wrapper function for all ghost item action logic.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for GhostItemActionLogic.
  ///
  /// Registers the required event subscribers directly (not data-driven) so
  /// that no FlatBuffers schema changes are needed.
  ///
  /// @param scene_context SceneContext instance providing all dependencies.
  /////////////////////////////////////////////////
  GhostItemActionLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns the LogicType of this Logic class.
  ///
  /// @return LogicType::GhostItemAction
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override {
    return LogicType::GhostItemAction;
  }
};

} // namespace steamrot::logic
