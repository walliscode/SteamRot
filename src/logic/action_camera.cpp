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
void apply_pan_start(CameraState &camera_state,
                     CameraPayload::PanDirection direction) {
  switch (direction) {
  case CameraPayload::PanDirection::UP:
    camera_state.m_panning_up = true;
    break;
  case CameraPayload::PanDirection::DOWN:
    camera_state.m_panning_down = true;
    break;
  case CameraPayload::PanDirection::LEFT:
    camera_state.m_panning_left = true;
    break;
  case CameraPayload::PanDirection::RIGHT:
    camera_state.m_panning_right = true;
    break;
  default:
    break;
  }
}

/////////////////////////////////////////////////
void apply_pan_stop(CameraState &camera_state,
                    CameraPayload::PanDirection direction) {
  switch (direction) {
  case CameraPayload::PanDirection::UP:
    camera_state.m_panning_up = false;
    break;
  case CameraPayload::PanDirection::DOWN:
    camera_state.m_panning_down = false;
    break;
  case CameraPayload::PanDirection::LEFT:
    camera_state.m_panning_left = false;
    break;
  case CameraPayload::PanDirection::RIGHT:
    camera_state.m_panning_right = false;
    break;
  default:
    break;
  }
}

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

    switch (camera_payload->action) {

    case CameraPayload::CameraAction::SCROLL:
      apply_zoom(camera_state, camera_payload->scroll_delta);
      break;

    case CameraPayload::CameraAction::RESET_ZOOM:
      reset_zoom(camera_state);
      break;

    case CameraPayload::CameraAction::PAN_PRESS:
      apply_pan_start(camera_state, camera_payload->m_pan_direction);
      break;

    case CameraPayload::CameraAction::PAN_RELEASE:
      apply_pan_stop(camera_state, camera_payload->m_pan_direction);
      break;

    default:
      break;
    }
  }
}

} // namespace steamrot::logic::action::camera
