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
#include "descriptors_runner.h"
#include "machina_form_scaffolds/machina_form_scaffold_library.h"
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

  case LogicPayload::LogicToggle::NONE:
    break;

  case LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD: {
    initialise_active_machina_form_scaffold(grimoire_machina);
    break;
  }

  case LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD: {
    clear_active_machina_form_scaffold(grimoire_machina);
    break;
  }
  case LogicPayload::LogicToggle::PERFORM_STRUCTURAL_ANALYSIS: {
    // if no active scaffold, break
    MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();

    if (!scaffold)
      break;

    descriptors::run_structural_analysis(scaffold->parts,
                                         scaffold->structural_analysis_results);

    // update the StucturalAnalysisState
    if (scaffold->structural_analysis_results.empty()) {
      scaffold->structural_analysis_state =
          StructuralAnalysisState::NothingFound;
    } else {
      scaffold->structural_analysis_state = StructuralAnalysisState::Found;
    }
    break;
  }
  case LogicPayload::LogicToggle::
      POPULATE_GRIMOIRE_MACHINA_WITH_GRAB_SCAFFOLD: {
    auto create_grabe_scaffold_result =
        create_grab_scaffold_one(grimoire_machina);
  }
  }
}
/////////////////////////////////////////////////
void place_first_piece(GrimoireMachina &grimoire_machina,
                       const MrGhost &mr_ghost) {
  // if no active scaffold, return
  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
  if (!scaffold)
    return;

  // if scaffold already has pieces, return
  if (!scaffold->parts.empty())
    return;

  // deal with FragmentInstance selection
  if (std::holds_alternative<FragmentInstance>(mr_ghost.m_instance)) {
    const FragmentInstance &ghost_fi =
        std::get<FragmentInstance>(mr_ghost.m_instance);

    // create a new FragmentInstance from the ghost selection, assign it the
    // next available stable ID, and add it to the scaffold's PartGraph
    FragmentInstance instance{scaffold->next_id++, ghost_fi.GetPart()};
    scaffold->parts.emplace(instance.GetId(), std::move(instance));

    // deal with JointInstance selection
  } else if (std::holds_alternative<JointInstance>(mr_ghost.m_instance)) {
    const JointInstance &ghost_ji =
        std::get<JointInstance>(mr_ghost.m_instance);

    // create a new JointInstance from the ghost selection, assign it the next
    // available stable ID, and add it to the scaffold's PartGraph
    JointInstance instance{scaffold->next_id++, ghost_ji.GetPart()};
    scaffold->parts.emplace(instance.GetId(), std::move(instance));
  }
}

/////////////////////////////////////////////////
void place_next_piece(MachinaFormScaffold &scaffold, const MrGhost &mr_ghost) {

  if (scaffold.parts.empty())
    return;

  auto ghost_socket_index = check_MrGhost_for_connection_readiness(mr_ghost);
  if (!ghost_socket_index.has_value())
    return;

  auto partgraph_result =
      check_PartGraph_for_connection_readiness(scaffold.parts);
  if (!partgraph_result.has_value())
    return;

  const uint32_t partgraph_part_id = partgraph_result.value().first;
  const uint32_t partgraph_socket_id = partgraph_result.value().second;

  if (std::holds_alternative<FragmentInstance>(mr_ghost.m_instance)) {
    const FragmentInstance &ghost_fi =
        std::get<FragmentInstance>(mr_ghost.m_instance);

    if (!std::holds_alternative<JointInstance>(
            scaffold.parts.at(partgraph_part_id)))
      return;

    FragmentInstance instance{scaffold.next_id++, ghost_fi.GetPart()};
    scaffold.parts.emplace(instance.GetId(), std::move(instance));

    FragmentInstance &placed_fi =
        std::get<FragmentInstance>(scaffold.parts.at(instance.GetId()));
    JointInstance &existing_ji =
        std::get<JointInstance>(scaffold.parts.at(partgraph_part_id));

    auto conn_result = placed_fi.CreateConnectionWithOtherInstance(
        ghost_socket_index.value(), existing_ji, partgraph_socket_id);

  } else if (std::holds_alternative<JointInstance>(mr_ghost.m_instance)) {
    const JointInstance &ghost_ji =
        std::get<JointInstance>(mr_ghost.m_instance);

    if (!std::holds_alternative<FragmentInstance>(
            scaffold.parts.at(partgraph_part_id)))
      return;

    JointInstance instance{scaffold.next_id++, ghost_ji.GetPart()};
    scaffold.parts.emplace(instance.GetId(), std::move(instance));

    JointInstance &placed_ji =
        std::get<JointInstance>(scaffold.parts.at(instance.GetId()));
    FragmentInstance &existing_fi =
        std::get<FragmentInstance>(scaffold.parts.at(partgraph_part_id));

    auto conn_result = placed_ji.CreateConnectionWithOtherInstance(
        ghost_socket_index.value(), existing_fi, partgraph_socket_id);
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
std::optional<uint32_t>
check_MrGhost_for_connection_readiness(const MrGhost &mr_ghost) {

  // if the ghost selection is empty, return nullopt
  if (std::holds_alternative<std::monostate>(mr_ghost.m_instance)) {
    return std::nullopt;

  } else if (std::holds_alternative<FragmentInstance>(mr_ghost.m_instance)) {

    FragmentInstance ghost_fi = std::get<FragmentInstance>(mr_ghost.m_instance);
    return ghost_fi.CheckForConnectionReadiness();
  } else if (std::holds_alternative<JointInstance>(mr_ghost.m_instance)) {

    JointInstance ghost_ji = std::get<JointInstance>(mr_ghost.m_instance);
    return ghost_ji.CheckForConnectionReadiness();
  }
  return std::nullopt;
}

/////////////////////////////////////////////////
std::optional<std::pair<uint32_t, uint32_t>>
check_PartGraph_for_connection_readiness(const PartGraph &part_graph) {

  // return false if empty
  if (part_graph.empty())
    return std::nullopt;

  // cycle through all parts in the PartGraph and check their sockets
  for (const auto &[id, part] : part_graph) {

    // deal with FragmentInstance
    if (std::holds_alternative<FragmentInstance>(part)) {

      const FragmentInstance &fi = std::get<FragmentInstance>(part);
      auto readiness_result = fi.CheckForConnectionReadiness();
      if (readiness_result.has_value())
        return std::make_pair(id, readiness_result.value());
    }

    else if (std::holds_alternative<JointInstance>(part)) {
      const JointInstance &ji = std::get<JointInstance>(part);

      auto readiness_result = ji.CheckForConnectionReadiness();
      if (readiness_result.has_value())
        return std::make_pair(id, readiness_result.value());
    }
  }
  return std::nullopt;
}

/////////////////////////////////////////////////
std::optional<JointFragmentConnection>
check_for_connected_sockets(const JointInstance &joint_instance,
                            const FragmentInstance &fragment_instance) {

  // cycle through all sockets on the JointInstance and check if any are
  // connected
  const uint32_t joint_id = joint_instance.GetId();
  for (const auto &[joint_socket_id, joint_socket_data] :
       joint_instance.GetSockets()) {
    if (joint_socket_data.GetConnectionState() ==
        SocketConnectionState::Connected) {

      // check if the connected socket is on the FragmentInstance
      if (joint_socket_data.GetConnection().has_value() &&
          joint_socket_data.GetConnection()->peer_part_id ==
              fragment_instance.GetId()) {

        // find the corresponding socket on the FragmentInstance
        uint32_t fragment_socket_id =
            joint_socket_data.GetConnection()->peer_socket_id;

        if (fragment_instance.TryGetSocket(fragment_socket_id) &&
            fragment_instance.TryGetSocket(fragment_socket_id)
                    ->GetConnectionState() ==
                SocketConnectionState::Connected) {

          const SocketState *fragment_socket =
              fragment_instance.TryGetSocket(fragment_socket_id);

          // a final check to ensure the FragmentInstance socket is connected
          // back to the JointInstance socket
          if (fragment_socket->GetConnection() &&
              fragment_socket->GetConnection()->peer_part_id == joint_id &&
              fragment_socket->GetConnection()->peer_socket_id ==
                  joint_socket_id) {

            return JointFragmentConnection{joint_id, joint_socket_id,
                                           fragment_instance.GetId(),
                                           fragment_socket_id};
          }
        }
      }
    }
  }
  return std::nullopt;
}

} // namespace steamrot::logic::action::grimoire_machina
