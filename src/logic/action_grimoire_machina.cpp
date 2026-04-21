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
#include <expected>
#include <string>
#include <vector>

namespace steamrot::logic::action::grimoire_machina {
/////////////////////////////////////////////////
void initialise_active_machina_form_scaffold(
    GrimoireMachina &grimoire_machina) {

  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form.reset();
  // add a new MachinaForm to the active form
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();
}

/////////////////////////////////////////////////
void clear_active_machina_form_scaffold(GrimoireMachina &grimoire_machina) {
  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form = nullptr;
}

/////////////////////////////////////////////////
void toggle_socket_visibility(MachinaFormScaffold &scaffold) {

  scaffold.are_sockets_visible = !scaffold.are_sockets_visible;
}
/////////////////////////////////////////////////
std::vector<std::string>
get_all_fragment_names(GrimoireMachina &grimoire_machina) {

  std::vector<std::string> fragment_names;
  // cycle through all fragments in the GrimoireMachina and add their names to
  // the vector
  for (const auto &[name, fragment] : grimoire_machina.m_all_fragments) {
    fragment_names.push_back(name);
  }
  return fragment_names;
}

/////////////////////////////////////////////////
std::vector<std::string>
get_all_joint_names(GrimoireMachina &grimoire_machina) {

  std::vector<std::string> joint_names;
  // cycle through all joints in the GrimoireMachina and add their names to
  // the vector
  for (const auto &[name, joint] : grimoire_machina.m_all_joints) {
    joint_names.push_back(name);
  }
  return joint_names;
}

/////////////////////////////////////////////////
void process_logic_events(Subscriber &subscriber,
                          GrimoireMachina &grimoire_machina) {
  if (!subscriber.captured_payload.has_value())
    return;

  const LogicPayload *logic_payload =
      std::get_if<LogicPayload>(&subscriber.captured_payload.value());
  if (!logic_payload)
    return;

  switch (logic_payload->toggle_name) {

  case LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD: {
    initialise_active_machina_form_scaffold(grimoire_machina);
    break;
  }

  case LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD: {
    clear_active_machina_form_scaffold(grimoire_machina);
    break;
  }

  default:
    break;
  }
}
/////////////////////////////////////////////////
void place_first_piece(GrimoireMachina &grimoire_machina,
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
void place_ghost_on_scaffold(GrimoireMachina &grimoire_machina,
                             const MrGhost &mr_ghost) {

  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
  if (!scaffold)
    return;

  if (scaffold->parts.empty()) {
    place_first_piece(grimoire_machina, mr_ghost);
    return;
  }

  // [TODO:] Socket-proximity collision detection will determine whether the
  // new piece can connect to an existing open socket on the scaffold.
}

/////////////////////////////////////////////////
void proces_user_input_events(Subscriber &subscriber,
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
    place_ghost_on_scaffold(grimoire_machina, scene_context.mr_ghost);
    break;

  case InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY:
    if (grimoire_machina.m_scaffold_form)
      toggle_socket_visibility(*grimoire_machina.m_scaffold_form);
    break;

  default:
    break;
  }
}

/////////////////////////////////////////////////
void process_subscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    const SceneContext &scene_context, GrimoireMachina &grimoire_machina) {
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;

    if (subscriber->event_type == EventType::LOGIC)
      process_logic_events(*subscriber, grimoire_machina);

    else if (subscriber->event_type == EventType::USER_INPUT)
      proces_user_input_events(*subscriber, scene_context, grimoire_machina);
  }
}
/////////////////////////////////////////////////
std::expected<Connection, std::string>
create_connection(FragmentInstance &fragment_instance, size_t socket_index_a,
                  JointInstance &joint_instance, size_t socket_index_b) {

  if (fragment_instance.sockets.empty() && joint_instance.sockets.empty())
    return std::unexpected(
        "Connection creation failed: both parts have no sockets.");

  if (socket_index_a >= fragment_instance.sockets.size() ||
      socket_index_b >= joint_instance.sockets.size())
    return std::unexpected(
        "Connection creation failed: one or both socket indices are out of "
        "range.");

  // modify the socket data on the FragmentInstance and JointInstance to reflect
  // the new connection
  fragment_instance.sockets[socket_index_a].state = SocketState::Connected;
  joint_instance.sockets[socket_index_b].state = SocketState::Connected;

  return Connection{Connection::Endpoint{fragment_instance.id, socket_index_a},
                    Connection::Endpoint{joint_instance.id, socket_index_b}};
}

} // namespace steamrot::logic::action::grimoire_machina
