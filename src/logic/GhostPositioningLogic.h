/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the GhostPositioningLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Logic.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
/// @class GhostPositioningLogic
/// @brief Updates the position of MrGhost to track the current mouse position.
///
/// Each tick this Logic writes the current mouse cursor position
/// (m_scene_context.mouse_position) into MrGhost::m_position so that other
/// Logic classes can use it to render or place the ghost item at the cursor.
/////////////////////////////////////////////////
class GhostPositioningLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Wrapper function for the logic processing
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for the GhostPositioningLogic class.
  ///
  /// @param scene_context Reference to a SceneContext struct instance to be
  /// used by the logic
  /////////////////////////////////////////////////
  GhostPositioningLogic(const SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns the LogicType enum value corresponding to this Logic.
  ///
  /// @return LogicType::GhostPositioning
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override {
    return LogicType::GhostPositioning;
  }
};

} // namespace steamrot::logic
