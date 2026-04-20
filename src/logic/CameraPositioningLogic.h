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
/// @brief Positioning logic that applies camera-based events to the camera state.
///
/// Subscribes to EventType::CAMERA via logic_config.json data-driven
/// configuration. Each tick that a CAMERA event is active the subscriber
/// captures the CameraPayload and applies its scroll delta to
/// SceneContext::camera_state via positioning::camera::ApplyZoom.
/////////////////////////////////////////////////
class CameraPositioningLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Process all active CAMERA subscribers.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CameraPositioningLogic.
  ///
  /// @param scene_context SceneContext providing event_handler and
  ///                      camera_state references.
  /////////////////////////////////////////////////
  CameraPositioningLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns LogicType::CameraPositioning.
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override { return LogicType::CameraPositioning; }
};

} // namespace steamrot::logic
