/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CameraMovementLogic class.
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
/// @class CameraMovementLogic
/// @brief Movement logic that applies camera-based events to the camera state.
///
/// Subscribes to EventType::CAMERA via logic_config.json data-driven
/// configuration. Each tick that a CAMERA event is active the subscriber
/// captures the CameraPayload and applies its scroll delta to
/// SceneContext::camera_state via movement::camera::ApplyZoom.
/////////////////////////////////////////////////
class CameraMovementLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Process all active CAMERA subscribers.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CameraMovementLogic.
  ///
  /// @param scene_context SceneContext providing event_handler and
  ///                      camera_state references.
  /////////////////////////////////////////////////
  CameraMovementLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns LogicType::CameraMovement.
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override { return LogicType::CameraMovement; }
};

} // namespace steamrot::logic
