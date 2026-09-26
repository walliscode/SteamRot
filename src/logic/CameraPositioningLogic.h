/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CameraPositioningLogic class.
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
/// @class CameraPositioningLogic
/// @brief Positioning logic that finalizes camera movement for the current tick.
///
/// Each tick this logic applies continuous camera pan movement from the current
/// CameraState flags, then computes the scene-wide authoritative
/// SceneContext::world_mouse_position from the finalized camera state and the
/// current screen-space mouse cursor.
/////////////////////////////////////////////////
class CameraPositioningLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Apply camera pan and refresh the authoritative world mouse position.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CameraPositioningLogic.
  ///
  /// @param scene_context SceneContext providing camera and mouse state
  ///                      references.
  /////////////////////////////////////////////////
  CameraPositioningLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns LogicType::CameraPositioning.
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override { return LogicType::CameraPositioning; }
};

} // namespace steamrot::logic
