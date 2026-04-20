/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for processing camera actions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_camera.h"
#include "EventPayload.h"

namespace steamrot::logic::action::camera {

/////////////////////////////////////////////////
void apply_zoom(CameraState &camera_state, float scroll_delta) {
  camera_state.m_zoom_level *= (1.0f - scroll_delta * CameraState::kZoomStep);
  camera_state.m_zoom_level = std::clamp(
      camera_state.m_zoom_level, CameraState::kMinZoom, CameraState::kMaxZoom);
}

/////////////////////////////////////////////////
void reset_zoom(CameraState &camera_state) { camera_state.m_zoom_level = 1.0f; }

/////////////////////////////////////////////////
void process_subscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    CameraState &camera_state) {
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;

    if (!subscriber->captured_payload.has_value())
      continue;

    const auto *camera_payload =
        std::get_if<CameraPayload>(&subscriber->captured_payload.value());
    if (!camera_payload)
      continue;

    apply_zoom(camera_state, camera_payload->scroll_delta);
  }
}

} // namespace steamrot::logic::action::camera
