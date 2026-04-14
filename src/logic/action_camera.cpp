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
void ProcessSubscribers(
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

    camera_state.ApplyScrollDelta(camera_payload->scroll_delta);
  }
}

} // namespace steamrot::logic::action::camera
