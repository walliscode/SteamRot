/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ResizeLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ResizeLogic.h"
#include "EventPayload.h"
#include "EventType.h"
#include "Subscriber.h"
#include "resize_logic.h"
#include <memory>

namespace steamrot::logic {

/////////////////////////////////////////////////
ResizeLogic::ResizeLogic(const SceneContext scene_context)
    : Logic(scene_context) {
  // Register a subscriber for SYSTEM RESIZE events in the constructor.
  // This subscriber is not data-driven since resize is a system-level
  // concern that does not vary per scene configuration.
  auto subscriber = std::make_shared<Subscriber>();
  subscriber->event_type = EventType::SYSTEM;
  subscriber->filter_payload =
      SystemPayload{SystemPayload::SystemAction::RESIZE};
  m_subscribers.push_back(subscriber);
  // Register with the EventHandler so it receives RESIZE events.
  m_scene_context.event_handler.RegisterSubscriber(subscriber);
}

/////////////////////////////////////////////////
void ResizeLogic::ProcessLogic() {
  for (const auto &subscriber : m_subscribers) {
    if (!subscriber->m_active) {
      continue;
    }

    if (!subscriber->captured_payload.has_value()) {
      continue;
    }

    if (!std::holds_alternative<SystemPayload>(
            subscriber->captured_payload.value())) {
      continue;
    }

    const SystemPayload &system_payload =
        std::get<SystemPayload>(subscriber->captured_payload.value());

    if (system_payload.action != SystemPayload::SystemAction::RESIZE) {
      continue;
    }

    if (!system_payload.optional_resize_size.has_value()) {
      continue;
    }

    resize::ResizeSceneTexture(m_scene_context.scene_texture,
                               system_payload.optional_resize_size.value());
  }
}

} // namespace steamrot::logic
