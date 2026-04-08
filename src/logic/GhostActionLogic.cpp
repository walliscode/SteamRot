/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GhostActionLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostActionLogic.h"
#include "EventPayload.h"
#include "EventType.h"
#include "Subscriber.h"
#include "action_ghost.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
GhostActionLogic::GhostActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {

  // Register a subscriber for GHOST SELECT events. The monostate filter
  // selection acts as a wildcard, matching any incoming SELECT payload
  // regardless of the concrete GhostSelection type.
  auto select_subscriber = std::make_shared<Subscriber>();
  select_subscriber->event_type = EventType::GHOST;
  select_subscriber->filter_payload =
      GhostPayload{GhostPayload::GhostAction::SELECT, std::monostate{}};
  m_subscribers.push_back(select_subscriber);
  m_scene_context.event_handler.RegisterSubscriber(select_subscriber);

  // Register a subscriber for GHOST CLEAR events.
  auto clear_subscriber = std::make_shared<Subscriber>();
  clear_subscriber->event_type = EventType::GHOST;
  clear_subscriber->filter_payload =
      GhostPayload{GhostPayload::GhostAction::CLEAR, std::monostate{}};
  m_subscribers.push_back(clear_subscriber);
  m_scene_context.event_handler.RegisterSubscriber(clear_subscriber);
}

/////////////////////////////////////////////////
void GhostActionLogic::ProcessLogic() {

  // Iterate active GHOST subscribers and dispatch to the correct free function
  // based on the GhostPayload::GhostAction enum value.
  for (auto &subscriber : m_subscribers) {
    if (!subscriber->m_active) {
      continue;
    }

    if (!subscriber->captured_payload.has_value()) {
      continue;
    }

    if (!std::holds_alternative<GhostPayload>(
            subscriber->captured_payload.value())) {
      continue;
    }

    const GhostPayload &ghost_payload =
        std::get<GhostPayload>(subscriber->captured_payload.value());

    switch (ghost_payload.action) {
    case GhostPayload::GhostAction::SELECT:
      action::ghost::SelectGhostItem(m_scene_context.mr_ghost,
                                     ghost_payload.m_selection);
      break;
    case GhostPayload::GhostAction::CLEAR:
    case GhostPayload::GhostAction::NONE:
      action::ghost::ClearGhostSelection(m_scene_context.mr_ghost);
      break;
    default:
      break;
    }
  }
}

} // namespace steamrot::logic
