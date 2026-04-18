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
#include "EventType.h"
#include "MachinaFormScaffold.h"
#include <string>
#include <vector>

namespace steamrot::logic::action::grimoire_machina {
/////////////////////////////////////////////////
void InitialiseActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina) {

  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form.reset();
  // add a new MachinaForm to the active form
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();
}

/////////////////////////////////////////////////
void ClearActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina) {
  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form = nullptr;
}

/////////////////////////////////////////////////
void ToggleSocketVisibility(MachinaFormScaffold &scaffold) {

  scaffold.are_sockets_visible = !scaffold.are_sockets_visible;
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
void ProcessLogicEvents(Subscriber &subscriber,
                        GrimoireMachina &grimoire_machina) {
  if (!subscriber.captured_payload.has_value())
    return;

  const LogicPayload *logic_payload =
      std::get_if<LogicPayload>(&subscriber.captured_payload.value());
  if (!logic_payload)
    return;

  switch (logic_payload->toggle_name) {

  case LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD: {
    InitialiseActiveMachinaFormScaffold(grimoire_machina);
    break;
  }

  case LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD: {
    ClearActiveMachinaFormScaffold(grimoire_machina);
    break;
  }

  default:
    break;
  }
}
/////////////////////////////////////////////////
void PlaceFirstPiece(GrimoireMachina &grimoire_machina,
                     const MrGhost &mr_ghost) {

  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
  if (!scaffold)
    return;

  if (!scaffold->parts.empty())
    return;

  if (std::holds_alternative<FragmentInstance>(mr_ghost.m_instance)) {
    const FragmentInstance &ghost_fi =
        std::get<FragmentInstance>(mr_ghost.m_instance);
    if (!ghost_fi.fragment)
      return;

    FragmentInstance instance{ghost_fi};
    const uint32_t id = scaffold->next_id++;
    instance.id = id;
    scaffold->parts.emplace(id, std::move(instance));

  } else if (std::holds_alternative<JointInstance>(mr_ghost.m_instance)) {
    const JointInstance &ghost_ji =
        std::get<JointInstance>(mr_ghost.m_instance);
    if (!ghost_ji.joint)
      return;

    JointInstance instance{ghost_ji};
    const uint32_t id = scaffold->next_id++;
    instance.id = id;
    scaffold->parts.emplace(id, std::move(instance));
  }
}

/////////////////////////////////////////////////
void PlaceGhostOnScaffold(GrimoireMachina &grimoire_machina,
                          const MrGhost &mr_ghost) {

  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
  if (!scaffold)
    return;

  if (scaffold->parts.empty()) {
    PlaceFirstPiece(grimoire_machina, mr_ghost);
    return;
  }

  // [TODO:] Socket-proximity collision detection will determine whether the
  // new piece can connect to an existing open socket on the scaffold.
}

/////////////////////////////////////////////////
void ProcessUserInputEvents(Subscriber &subscriber,
                            const SceneContext &scene_context,
                            GrimoireMachina &grimoire_machina) {

  if (!subscriber.captured_payload.has_value())
    return;

  const InputPayload *input_payload =
      std::get_if<InputPayload>(&subscriber.captured_payload.value());
  if (!input_payload)
    return;

  switch (input_payload->action) {

  case InputPayload::InputAction::SELECT:

    // deal with Ghost placement on scaffold when SELECT action is triggered,
    // checking guards in order checking for empty selection
    if (std::holds_alternative<std::monostate>(
            scene_context.mr_ghost.m_instance))
      break;
    // if mouse is hovering over UI, do not place piece on scaffold
    if (scene_context.scene_state.is_mouse_over_ui_layer)
      break;
    PlaceGhostOnScaffold(grimoire_machina, scene_context.mr_ghost);
    break;

  case InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY:
    if (grimoire_machina.m_scaffold_form)
      ToggleSocketVisibility(*grimoire_machina.m_scaffold_form);
    break;

  default:
    break;
  }
}

/////////////////////////////////////////////////
void ProcessSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    const SceneContext &scene_context, GrimoireMachina &grimoire_machina) {
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;

    if (subscriber->event_type == EventType::LOGIC)
      ProcessLogicEvents(*subscriber, grimoire_machina);

    else if (subscriber->event_type == EventType::USER_INPUT)
      ProcessUserInputEvents(*subscriber, scene_context, grimoire_machina);
  }
}

} // namespace steamrot::logic::action::grimoire_machina
