/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CameraZoomActionLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraZoomActionLogic.h"
#include "EventPayload.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
CameraZoomActionLogic::CameraZoomActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void CameraZoomActionLogic::ProcessLogic() {
  for (auto &subscriber : m_subscribers) {
    if (!subscriber->m_active)
      continue;

    if (!subscriber->captured_payload.has_value())
      continue;

    const auto *camera_payload =
        std::get_if<CameraPayload>(&subscriber->captured_payload.value());
    if (!camera_payload)
      continue;

    m_scene_context.camera_state.ApplyScrollDelta(camera_payload->scroll_delta);
  }
}

} // namespace steamrot::logic
