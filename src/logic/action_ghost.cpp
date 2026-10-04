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
#include "overload.h"

namespace steamrot::logic::action::ghost {

/////////////////////////////////////////////////
void select_ghost_item(MrGhost &mr_ghost, const GhostSelection &selection,
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
      // create a new JointInstance
      JointInstance joint_instance(0, it->second);
      // position the sockets on the Joint instance
      joint_instance.PositionSockets(
          JointSocketPositioningStrategy::MaximizeDistance);
      // add the JointInstance to the MrGhost instance variant
      mr_ghost.m_instance.emplace<JointInstance>(joint_instance);
    }
  }
}

/////////////////////////////////////////////////
void clear_ghost_selection(MrGhost &mr_ghost) {
  mr_ghost.m_instance = std::monostate{};
}

/////////////////////////////////////////////////
std::optional<uint32_t>
check_if_instance_is_connection_ready(const MrGhost &mr_ghost) {

  return std::visit(
      overload{
          [&](const FragmentInstance &frag_instance)
              -> std::optional<uint32_t> {
            return frag_instance.CheckIfAnySocketIsWithinConnectionDistance();
          },
          [&](const JointInstance &joint_instance) -> std::optional<uint32_t> {
            return joint_instance.CheckIfAnySocketIsWithinConnectionDistance();
          },
          [&](const std::monostate &) -> std::optional<uint32_t> {
            return std::nullopt;
          },
      },
      mr_ghost.m_instance);
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
    select_ghost_item(mr_ghost, ghost_payload.m_selection, asset_manager);
    break;

  case GhostPayload::GhostAction::CLEAR:
    clear_ghost_selection(mr_ghost);
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
