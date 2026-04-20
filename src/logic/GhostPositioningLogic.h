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
/// @brief Updates the position and rotation of MrGhost to track the current
/// mouse position and respond to rotation input.
///
/// Each tick this Logic processes active subscribers: if a ROTATE_GHOST input
/// event has been captured, it increments MrGhost::m_rotation_degrees by 90
/// degrees. It then writes the current mouse cursor position into
/// MrGhost::m_position and rebuilds the instance transform (including both
/// translation and the accumulated rotation) so that other Logic classes can
/// render the ghost item at the correct world position and orientation.
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
