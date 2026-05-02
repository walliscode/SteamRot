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
void place_next_piece(MachinaFormScaffold &scaffold, const MrGhost &mr_ghost) {

  if (scaffold.parts.empty())
    return;

  auto ghost_socket_index = check_MrGhost_for_connection_readiness(mr_ghost);
  if (!ghost_socket_index.has_value())
    return;

  auto partmap_result = check_PartMap_for_connection_readiness(scaffold.parts);
  if (!partmap_result.has_value())
    return;

  const uint32_t partmap_part_id = partmap_result.value().first;
  const size_t partmap_socket_idx = partmap_result.value().second;

  if (std::holds_alternative<FragmentInstance>(mr_ghost.m_instance)) {
    const FragmentInstance &ghost_fi =
        std::get<FragmentInstance>(mr_ghost.m_instance);
    if (!ghost_fi.fragment)
      return;

    if (!std::holds_alternative<JointInstance>(
            scaffold.parts.at(partmap_part_id)))
      return;

    FragmentInstance instance{ghost_fi};
    const uint32_t new_id = scaffold.next_id++;
    instance.id = new_id;
    scaffold.parts.emplace(new_id, std::move(instance));

    FragmentInstance &placed_fi =
        std::get<FragmentInstance>(scaffold.parts.at(new_id));
    JointInstance &existing_ji =
        std::get<JointInstance>(scaffold.parts.at(partmap_part_id));

    auto connection_result = create_connection(
        placed_fi, ghost_socket_index.value(), existing_ji, partmap_socket_idx);
    static_cast<void>(connection_result);

  } else if (std::holds_alternative<JointInstance>(mr_ghost.m_instance)) {
    const JointInstance &ghost_ji =
        std::get<JointInstance>(mr_ghost.m_instance);
    if (!ghost_ji.joint)
      return;

    if (!std::holds_alternative<FragmentInstance>(
            scaffold.parts.at(partmap_part_id)))
      return;

    JointInstance instance{ghost_ji};
    const uint32_t new_id = scaffold.next_id++;
    instance.id = new_id;
    scaffold.parts.emplace(new_id, std::move(instance));

    JointInstance &placed_ji =
        std::get<JointInstance>(scaffold.parts.at(new_id));
    FragmentInstance &existing_fi =
        std::get<FragmentInstance>(scaffold.parts.at(partmap_part_id));

    auto connection_result =
        create_connection(existing_fi, partmap_socket_idx, placed_ji,
                          ghost_socket_index.value());
    static_cast<void>(connection_result);
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

  place_next_piece(*scaffold, mr_ghost);
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
std::expected<std::monostate, std::string>
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

  // Mark both sockets as Connected and store the reciprocal peer reference.
  fragment_instance.sockets[socket_index_a].state = SocketState::Connected;
  fragment_instance.sockets[socket_index_a].connected_to =
      SocketConnection{joint_instance.id, socket_index_b};

  joint_instance.sockets[socket_index_b].state = SocketState::Connected;
  joint_instance.sockets[socket_index_b].connected_to =
      SocketConnection{fragment_instance.id, socket_index_a};

  return std::monostate{};
}

/////////////////////////////////////////////////
bool check_socket_for_connection_readiness(const SocketData &socket) {

  if (socket.state != SocketState::Available || !socket.is_ready_to_connect) {
    return false;
  } else {
    return true;
  }
}
/////////////////////////////////////////////////
std::optional<size_t>
check_MrGhost_for_connection_readiness(const MrGhost &mr_ghost) {

  // if the ghost selection is empty, return nullopt
  if (std::holds_alternative<std::monostate>(mr_ghost.m_instance))
    return std::nullopt;

  // deal with FragmentInstance
  if (std::holds_alternative<FragmentInstance>(mr_ghost.m_instance)) {
    const FragmentInstance &ghost_fi =
        std::get<FragmentInstance>(mr_ghost.m_instance);

    // if the ghost selection is a FragmentInstance with no fragment or no
    // sockets, return nullopt
    if (!ghost_fi.fragment || ghost_fi.sockets.empty())
      return std::nullopt;

    // find first socket that is ready for connection and return its index, if
    // no sockets
    auto it = std::find_if(ghost_fi.sockets.begin(), ghost_fi.sockets.end(),
                           check_socket_for_connection_readiness);

    // if no sockets are ready for connection, return nullopt
    if (it == ghost_fi.sockets.end())
      return std::nullopt;

    // otherwise, return the index of the first socket that is ready for
    // connection
    return std::distance(ghost_fi.sockets.begin(), it);

  } else if (std::holds_alternative<JointInstance>(mr_ghost.m_instance)) {
    const JointInstance &ghost_ji =
        std::get<JointInstance>(mr_ghost.m_instance);

    // if the ghost selection is a JointInstance with no joint or no sockets,
    // return nullopt
    if (!ghost_ji.joint || ghost_ji.sockets.empty())
      return std::nullopt;

    // find first socket that is ready for connection and return its index, if
    // no sockets
    auto it = std::find_if(ghost_ji.sockets.begin(), ghost_ji.sockets.end(),
                           check_socket_for_connection_readiness);
    // if no sockets are ready for connection, return nullopt
    if (it == ghost_ji.sockets.end())
      return std::nullopt;

    // otherwise, return the index of the first socket that is ready for
    // connection

    return std::distance(ghost_ji.sockets.begin(), it);
  }
  return std::nullopt;
}

std::optional<std::pair<uint32_t, size_t>>
check_PartMap_for_connection_readiness(const PartMap &part_map) {

  // return false if empty
  if (part_map.empty())
    return std::nullopt;

  // cycle through all parts in the PartMap and check their sockets
  for (const auto &[id, part] : part_map) {

    // check if the part is a FragmentInstance and if any of its sockets are
    // ready
    if (std::holds_alternative<FragmentInstance>(part)) {
      const FragmentInstance &fi = std::get<FragmentInstance>(part);

      // cycle throught the sockets and check
      auto it = std::find_if(fi.sockets.begin(), fi.sockets.end(),
                             check_socket_for_connection_readiness);

      // if successful, return the part id and socket index of the first ready
      // socket
      if (it != fi.sockets.end()) {
        size_t socket_index = std::distance(fi.sockets.begin(), it);
        return std::make_pair(id, socket_index);
      }

      // check if the part is a JointInstance and if any of its sockets are
      // ready
    } else if (std::holds_alternative<JointInstance>(part)) {
      const JointInstance &ji = std::get<JointInstance>(part);

      // cycle throught the sockets and check
      auto it = std::find_if(ji.sockets.begin(), ji.sockets.end(),
                             check_socket_for_connection_readiness);

      // if successful, return the part id and socket index of the first ready
      // socket
      if (it != ji.sockets.end()) {
        size_t socket_index = std::distance(ji.sockets.begin(), it);
        return std::make_pair(id, socket_index);
      }
    }
  }

  return std::nullopt;
}

} // namespace steamrot::logic::action::grimoire_machina
