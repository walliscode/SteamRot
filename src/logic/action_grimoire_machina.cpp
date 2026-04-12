/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for process actions related to the
/// GrimoireMachina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "EventPayload.h"
#include "MachinaFormScaffold.h"
#include "ViewDirection.h"
#include <SFML/Graphics/Transform.hpp>
#include <string>
#include <vector>

namespace steamrot::logic::action::grimoire_machina {
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InitialiseActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina) {

  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form.reset();
  // add a new MachinaForm to the active form
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ClearActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina) {
  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form = nullptr;

  return std::monostate{};
}

/////////////////////////////////////////////////
std::vector<std::string>
GetAllFragmentNames(GrimoireMachina &grimoire_machina) {

  std::vector<std::string> fragment_names;
  // cycle through all fragments in the GrimoireMachina and add their names to
  // the vector
  for (const auto &[name, fragment] : grimoire_machina.m_all_fragments) {
    fragment_names.push_back(name);
  }
  return fragment_names;
}

/////////////////////////////////////////////////
std::vector<std::string> GetAllJointNames(GrimoireMachina &grimoire_machina) {

  std::vector<std::string> joint_names;
  // cycle through all joints in the GrimoireMachina and add their names to
  // the vector
  for (const auto &[name, joint] : grimoire_machina.m_all_joints) {
    joint_names.push_back(name);
  }
  return joint_names;
}

/////////////////////////////////////////////////
void ProcessSubscriber(Subscriber &subscriber,
                       GrimoireMachina &grimoire_machina) {
  if (!subscriber.captured_payload.has_value())
    return;

  const LogicPayload *logic_payload =
      std::get_if<LogicPayload>(&subscriber.captured_payload.value());
  if (!logic_payload)
    return;

  if (logic_payload->toggle_name ==
      LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD) {
    // [TODO:] handle the result of this action and report failure if it fails.
    auto initialise_result =
        InitialiseActiveMachinaFormScaffold(grimoire_machina);
  } else if (logic_payload->toggle_name ==
             LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD) {
    // [TODO:] handle the result of this action and report failure if it fails.
    auto clear_result = ClearActiveMachinaFormScaffold(grimoire_machina);
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
PlaceGhostOnScaffold(GrimoireMachina &grimoire_machina, const MrGhost &mr_ghost,
                     sf::Vector2f world_pos) {

  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
  if (!scaffold)
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "PlaceGhostOnScaffold: no active scaffold"});

  // Matches the anchor used by the ghost renderer so the placed piece
  // visually snaps to where the ghost preview was hovering.
  static constexpr float k_corner_offset = 5.f;

  if (std::holds_alternative<FragmentTag>(mr_ghost.m_selection)) {
    const auto &tag = std::get<FragmentTag>(mr_ghost.m_selection);
    auto it = grimoire_machina.m_all_fragments.find(tag.key);
    if (it == grimoire_machina.m_all_fragments.end())
      return std::unexpected(
          FailInfo{FailMode::MissingData,
                   "PlaceGhostOnScaffold: fragment key not found"});

    Fragment &fragment = it->second;
    const sf::FloatRect bounds =
        fragment.movement_views[ViewDirection::Front].getBounds();

    // Translate so the placed piece appears at the same position the ghost
    // was rendered (matching the ghost anchor: bottom-right corner of bounds).
    sf::Transform transform;
    transform.translate(world_pos - bounds.position - bounds.size -
                        sf::Vector2f{k_corner_offset, k_corner_offset});

    FragmentInstance instance{fragment, transform};
    instance.id = scaffold->next_id++;
    scaffold->fragments.push_back(std::move(instance));

  } else if (std::holds_alternative<JointTag>(mr_ghost.m_selection)) {
    const auto &tag = std::get<JointTag>(mr_ghost.m_selection);
    auto it = grimoire_machina.m_all_joints.find(tag.key);
    if (it == grimoire_machina.m_all_joints.end())
      return std::unexpected(
          FailInfo{FailMode::MissingData,
                   "PlaceGhostOnScaffold: joint key not found"});

    Joint &joint = it->second;
    const sf::FloatRect bounds =
        joint.movement_views[ViewDirection::Front].getBounds();

    sf::Transform transform;
    transform.translate(world_pos - bounds.position - bounds.size -
                        sf::Vector2f{k_corner_offset, k_corner_offset});

    JointInstance instance{joint, transform};
    instance.id = scaffold->next_id++;
    scaffold->joints.push_back(std::move(instance));

  } else {
    return std::unexpected(
        FailInfo{FailMode::InvalidInput,
                 "PlaceGhostOnScaffold: no ghost item selected"});
  }

  return std::monostate{};
}

} // namespace steamrot::logic::action::grimoire_machina

