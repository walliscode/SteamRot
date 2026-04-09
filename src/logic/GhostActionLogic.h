/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the GhostActionLogic class.
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
/// @class GhostActionLogic
/// @brief Processes ghost-related action events and updates MrGhost state.
///
/// GhostActionLogic subscribes to GHOST events on the event bus and applies
/// incoming GhostPayloads to the scene's MrGhost instance. On each tick,
/// ProcessLogic iterates active subscribers, extracts the GhostPayload, and
/// switches on GhostPayload::GhostAction to call the appropriate action_ghost
/// free function (SelectGhostItem or ClearGhostSelection).
/////////////////////////////////////////////////
class GhostActionLogic : public Logic {

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for GhostActionLogic.
  ///
  /// Registers subscribers for GHOST SELECT and GHOST CLEAR events on the
  /// scene event handler so that ProcessLogic can react to selection changes
  /// each tick.
  ///
  /// @param scene_context SceneContext struct instance to be used by the logic
  /// class.
  /////////////////////////////////////////////////
  GhostActionLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns the LogicType enum value for this Logic instance.
  ///
  /// @return LogicType::GhostAction
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override { return LogicType::GhostAction; }

  /////////////////////////////////////////////////
  /// @brief Deal with active GHOST event subscribers.
  ///
  /// @param subscriber Subscriber to process and take info from
  /////////////////////////////////////////////////
  void ProcessGhostEvents(Subscriber &subscriber);

private:
  /////////////////////////////////////////////////
  /// @brief Dispatches GHOST event subscribers to the correct free function.
  ///
  /// Iterates active subscribers, extracts each GhostPayload, and switches on
  /// GhostPayload::GhostAction to call SelectGhostItem (SELECT) or
  /// ClearGhostSelection (CLEAR / NONE).
  /////////////////////////////////////////////////
  void ProcessLogic() override;
};

} // namespace steamrot::logic
