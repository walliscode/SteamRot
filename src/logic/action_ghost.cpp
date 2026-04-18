/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for processing actions related to
/// MrGhost.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_ghost.h"
#include "EventPayload.h"

namespace steamrot::logic::action::ghost {

/////////////////////////////////////////////////
void SelectGhostItem(MrGhost &mr_ghost, const GhostInstance &instance) {
  mr_ghost.m_instance = instance;
}

/////////////////////////////////////////////////
void ClearGhostSelection(MrGhost &mr_ghost) {
  mr_ghost.m_instance = std::monostate{};
}

/////////////////////////////////////////////////
void ProcessSubscriber(Subscriber &subscriber, MrGhost &mr_ghost) {
  if (!subscriber.captured_payload.has_value())
    return;

  if (!std::holds_alternative<GhostPayload>(
          subscriber.captured_payload.value()))
    return;

  const GhostPayload &ghost_payload =
      std::get<GhostPayload>(subscriber.captured_payload.value());

  switch (ghost_payload.action) {
  case GhostPayload::GhostAction::SELECT:
    SelectGhostItem(mr_ghost, ghost_payload.m_instance);
    break;

  case GhostPayload::GhostAction::CLEAR:
    ClearGhostSelection(mr_ghost);
    break;

  default:
    break;
  }
}

/////////////////////////////////////////////////
void ProcessSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    MrGhost &mr_ghost) {
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;
    ProcessSubscriber(*subscriber, mr_ghost);
  }
}

} // namespace steamrot::logic::action::ghost
