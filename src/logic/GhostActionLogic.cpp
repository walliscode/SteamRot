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
  auto select_reg_result =
      m_scene_context.event_handler.RegisterSubscriber(select_subscriber);

  // Register a subscriber for GHOST CLEAR events.
  auto clear_subscriber = std::make_shared<Subscriber>();
  clear_subscriber->event_type = EventType::GHOST;
  clear_subscriber->filter_payload =
      GhostPayload{GhostPayload::GhostAction::CLEAR, std::monostate{}};
  m_subscribers.push_back(clear_subscriber);
  auto clear_reg_result =
      m_scene_context.event_handler.RegisterSubscriber(clear_subscriber);
}

/////////////////////////////////////////////////
void GhostActionLogic::ProcessLogic() {
  for (auto &subscriber : m_subscribers) {
    if (!subscriber->m_active)
      continue;
    action::ghost::ProcessSubscriber(*subscriber, m_scene_context.mr_ghost);
  }
}

} // namespace steamrot::logic
