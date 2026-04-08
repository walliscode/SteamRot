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
/// incoming GhostPayloads to the scene's MrGhost instance. It also clears the
/// active GhostSelection when the Q key is pressed.
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

private:
  /////////////////////////////////////////////////
  /// @brief Processes GHOST event subscribers and Q key input each tick.
  ///
  /// Iterates active subscribers and calls the appropriate action_ghost free
  /// function to update MrGhost. Also clears the GhostSelection when the Q
  /// key is currently pressed.
  /////////////////////////////////////////////////
  void ProcessLogic() override;
};

} // namespace steamrot::logic
