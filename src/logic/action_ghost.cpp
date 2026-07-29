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
void SelectGhostItem(MrGhost &mr_ghost, const GhostSelection &selection,
                     AssetManager &asset_manager) {
  auto grimoire_result = asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value())
    return;
  GrimoireMachina *grimoire = grimoire_result.value();

  if (const auto *tag = std::get_if<FragmentTag>(&selection)) {
    auto it = grimoire->m_all_fragments.find(tag->key);
    if (it != grimoire->m_all_fragments.end())
      mr_ghost.m_instance.emplace<FragmentInstance>(0, it->second);

  } else if (const auto *tag = std::get_if<JointTag>(&selection)) {
    auto it = grimoire->m_all_joints.find(tag->key);
    if (it != grimoire->m_all_joints.end()) {
      mr_ghost.m_instance.emplace<JointInstance>(0, it->second);
    }
  }
}

/////////////////////////////////////////////////
void ClearGhostSelection(MrGhost &mr_ghost) {
  mr_ghost.m_instance = std::monostate{};
}

/////////////////////////////////////////////////
void ProcessSubscriber(Subscriber &subscriber, MrGhost &mr_ghost,
                       AssetManager &asset_manager) {
  if (!subscriber.captured_payload.has_value())
    return;

  if (!std::holds_alternative<GhostPayload>(
          subscriber.captured_payload.value()))
    return;

  const GhostPayload &ghost_payload =
      std::get<GhostPayload>(subscriber.captured_payload.value());

  switch (ghost_payload.action) {
  case GhostPayload::GhostAction::SELECT:
    SelectGhostItem(mr_ghost, ghost_payload.m_selection, asset_manager);
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
    MrGhost &mr_ghost, AssetManager &asset_manager) {
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;
    ProcessSubscriber(*subscriber, mr_ghost, asset_manager);
  }
}

} // namespace steamrot::logic::action::ghost
