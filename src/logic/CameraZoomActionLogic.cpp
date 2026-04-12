/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CameraZoomActionLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraZoomActionLogic.h"
#include "EventPayload.h"
#include "EventType.h"
#include "Subscriber.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
CameraZoomActionLogic::CameraZoomActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {

  // Register a subscriber for CAMERA scroll events.  Any CameraPayload
  // matches (the delta is read from captured_payload, not the filter).
  auto scroll_subscriber = std::make_shared<Subscriber>();
  scroll_subscriber->event_type = EventType::CAMERA;
  scroll_subscriber->filter_payload = CameraPayload{};
  m_subscribers.push_back(scroll_subscriber);
  m_scene_context.event_handler.RegisterSubscriber(scroll_subscriber);
}

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
