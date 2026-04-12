/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CameraZoomActionLogic class.
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
/// @class CameraZoomActionLogic
/// @brief Action logic that translates scroll-wheel events into camera zoom.
///
/// Subscribes to EventType::CAMERA via logic_config.json data-driven
/// configuration. Each tick that a CAMERA event is active the subscriber
/// captures the CameraPayload and applies its scroll delta to
/// SceneContext::camera_state.
/////////////////////////////////////////////////
class CameraZoomActionLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Process camera zoom from any active CAMERA subscriber.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CameraZoomActionLogic.
  ///
  /// @param scene_context SceneContext providing event_handler and
  ///                      camera_state references.
  /////////////////////////////////////////////////
  CameraZoomActionLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns LogicType::CameraZoom.
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override { return LogicType::CameraZoom; }
};

} // namespace steamrot::logic
