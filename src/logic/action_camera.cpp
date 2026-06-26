/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for processing camera actions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_camera.h"
#include "CameraState.h"
#include "EventPayload.h"
#include "positioning_grimoire_machina.h"

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
void align_camera_to_machina_form_scaffold(const MachinaFormScaffold &scaffold,
                                           CameraState &camera_state) {
  // get PartGraph from scaffold
  const PartGraph &part_graph = scaffold.parts;

  // if the PartGraph is empty, reset the camera to default position and zoom
  if (part_graph.empty()) {
    camera_state.m_position = {0.f, 0.f};
    camera_state.m_zoom_level = 1.0f;
    return;
  }

  // calculate the outer box of the PartGraph
  auto bounds =
      positioning::grimoire_machina::calculate_outer_box(part_graph, {}, false);

  // set the camera position to the center of the outer box
  camera_state.m_position = bounds.getCenter();
  // set the camera zoom level to 1.0f for now
  camera_state.m_zoom_level = 1.0f;
}

/////////////////////////////////////////////////
void process_subscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    CameraState &camera_state, MachinaFormScaffold *scaffold) {
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

    case CameraPayload::CameraAction::ALIGN_TO_MACHINA_FORM_SCAFFOLD:
      // Ensure scaffold is not null before aligning
      if (scaffold)
        align_camera_to_machina_form_scaffold(*scaffold, camera_state);
      break;

    case CameraPayload::CameraAction::NONE:
      break;
    }
  }
}

} // namespace steamrot::logic::action::camera
