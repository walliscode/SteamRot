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
#include "FragmentInstance.h"
#include "GrimoireMachina.h"
#include "JointInstance.h"
#include "MachinaFormScaffold.h"
#include "action_ghost.h"
#include "descriptors_runner.h"
#include "machina_form_scaffolds/machina_form_scaffold_library.h"
#include "overload.h"
#include <cstdint>
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

  grimoire_machina.m_scaffold_form = nullptr;
}

/////////////////////////////////////////////////
void toggle_socket_visibility(MachinaFormScaffold &scaffold) {

  scaffold.are_sockets_visible = !scaffold.are_sockets_visible;
}

/////////////////////////////////////////////////
uint32_t generate_stable_id(MachinaFormScaffold &scaffold) {

  return ++scaffold.next_id;
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
    if (scaffold->structural_analysis_results.successful_results.empty()) {
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
void place_first_piece(MachinaFormScaffold *scaffold, const MrGhost &mr_ghost) {
  // if no active scaffold, return
  if (!scaffold)
    return;

  // if scaffold already has pieces, return
  if (!scaffold->parts.empty())
    return;

  // we create a static variable for the position of the first piece. This is
  // ostensibly the middle of the CraftingScene
  static const sf::Vector2f first_piece_position{0.0f, 0.0f};

  // capture how to handle every variant of the MrGhost instance with std::visit
  // and overload pattern
  std::visit(
      overload{[&](const FragmentInstance &instance) {
                 // create a new FragmentInstance from the ghost
                 // selection, assign it the next available stable ID,
                 // and add it to the scaffold's PartGraph
                 const uint32_t next_id = scaffold->next_id++;
                 scaffold->parts.emplace(
                     next_id, FragmentInstance{next_id, instance.GetPart()});

                 // pull out reference to make any further modifications easier
                 FragmentInstance &new_fragment_instance =
                     std::get<FragmentInstance>(scaffold->parts.at(next_id));
                 // set position to first_piece_position
                 new_fragment_instance.setPosition(first_piece_position);
               },
               [&](const JointInstance &instance) {
                 // create a new JointInstance from the ghost selection, assign
                 // it the next available stable ID, and add it to the
                 // scaffold's PartGraph
                 const uint32_t next_id = scaffold->next_id++;
                 scaffold->parts.emplace(
                     next_id, JointInstance{next_id, instance.GetPart()});
                 // we need to make sure the new JointInstance has its sockets
                 // positioned correctly
                 JointInstance &new_joint_instance =
                     std::get<JointInstance>(scaffold->parts.at(next_id));
                 new_joint_instance.PositionSockets(
                     JointSocketPositioningStrategy::MaximizeDistance);

                 // set position to first_piece_position
                 new_joint_instance.setPosition(first_piece_position);
               },
               [](const std::monostate &) {
                 // std::monostate: nothing to update
               }},
      mr_ghost.m_instance);
}

/////////////////////////////////////////////////
void place_next_piece(MachinaFormScaffold *scaffold, const MrGhost &mr_ghost) {

  // if no active scaffold, return
  if (!scaffold)
    return;

  // this should not be handled with an empty scaffold
  if (scaffold->parts.empty())
    return;

  auto ghost_instnace_result =
      ghost::check_if_instance_is_connection_ready(mr_ghost);
  // return early if the ghost instance is not ready to connect
  if (!ghost_instnace_result.has_value())
    return;

  // pull out socket_id from the result
  const uint32_t ghost_socket_id = ghost_instnace_result.value();

  // get first ready socket from the PartGraph
  auto partgraph_result =
      check_part_graph_for_connection_readiness(scaffold->parts);
  if (!partgraph_result.has_value())
    return;

  // pull out the part_id and socket_id from the result
  const uint32_t partgraph_part_id = partgraph_result.value().first;
  const uint32_t partgraph_socket_id = partgraph_result.value().second;

  // capture dual variant interactions with std::visit and overload pattern
  // steps:
  // // create a new instance that is a copy of the ghost selection
  // // assign it the next available stable ID
  // // add it to the scaffold's PartGraph
  // // create a connection between the new instance and the existing instance
  // // align the new instance's socket with the existing instance's socket
  std::visit(
      overload{[&](const FragmentInstance &ghost_instance,
                   JointInstance &part_instance) {
                 // create a new FragmentInstance from the ghost
                 // selection, assign it
                 const FragmentInstance new_fragment_instance{
                     generate_stable_id(*scaffold), ghost_instance.GetPart()};
                 scaffold->parts.emplace(new_fragment_instance.GetId(),
                                         new_fragment_instance);
                 // pull out FragmentInstance reference to make any further
                 // modifications easier
                 FragmentInstance &new_fragment_instance_ref =
                     std::get<FragmentInstance>(
                         scaffold->parts.at(new_fragment_instance.GetId()));

                 // create a connection between the new FragmentInstance and the
                 // existing JointInstance
                 auto connection_result =
                     part_instance.CreateConnectionWithOtherInstance(
                         partgraph_socket_id, new_fragment_instance_ref,
                         ghost_socket_id);
                 // [TODO:] use the return value properly to check for errors
                 // and handle them. This may require a proper overhaul

                 // algin the new FragmentInstance's socket with the existing
                 // JointInstance's socket
                 auto align_result =
                     new_fragment_instance_ref.AlignOntoOtherPartInstance(
                         ghost_socket_id, part_instance, partgraph_socket_id);
               },
               [&](const JointInstance &ghost_instance,
                   FragmentInstance &part_instance) {
                 const JointInstance new_joint_instance{
                     generate_stable_id(*scaffold), ghost_instance.GetPart()};
                 scaffold->parts.emplace(new_joint_instance.GetId(),
                                         new_joint_instance);
                 // pull out JointInstance reference to make any further
                 // modifications easier
                 JointInstance &new_joint_instance_ref =
                     std::get<JointInstance>(
                         scaffold->parts.at(new_joint_instance.GetId()));
                 // position the new JointInstance's sockets
                 new_joint_instance_ref.PositionSockets(
                     JointSocketPositioningStrategy::MaximizeDistance);

                 // create a connection between the new JointInstance and the
                 // existing FragmentInstance
                 auto connection_result =
                     part_instance.CreateConnectionWithOtherInstance(
                         partgraph_socket_id, new_joint_instance_ref,
                         ghost_socket_id);
                 // [TODO:] use the return value properly to check for errors
                 // and handle and handle them. This may require a proper
                 // overhaul
                 // align the new JointInstance's socket with the existing
                 // FragmentInstance's socket
                 auto align_result =
                     new_joint_instance_ref.AlignOntoOtherPartInstance(
                         ghost_socket_id, part_instance, partgraph_socket_id);
               },

               [](const auto &, const auto &) {
                 // fall back case: if both are the same type, do nothing
                 // (or other types in the variant)
               }},
      mr_ghost.m_instance, scaffold->parts.at(partgraph_part_id));
}

/////////////////////////////////////////////////
void place_ghost_on_scaffold(GrimoireMachina &grimoire_machina,
                             MrGhost &mr_ghost) {

  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
  if (!scaffold)
    return;

  if (scaffold->parts.empty()) {
    place_first_piece(scaffold, mr_ghost);
    return;
  }

  place_next_piece(scaffold, mr_ghost);

  // clear the ghost selection after placing it on the scaffold
  // ghost::clear_ghost_selection(mr_ghost);
}

/////////////////////////////////////////////////
void process_user_input_events(Subscriber &subscriber,
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
      process_user_input_events(*subscriber, scene_context, grimoire_machina);
  }
}

/////////////////////////////////////////////////
std::optional<std::pair<uint32_t, uint32_t>>
check_part_graph_for_connection_readiness(const PartGraph &part_graph) {

  // cyycle through the PartGraph and check each PartInstance for a ready socket
  for (const auto &[part_id, part_instance] : part_graph) {
    auto result = std::visit(
        overload{
            [&](const FragmentInstance &fragment_instance)
                -> std::optional<std::pair<uint32_t, uint32_t>> {
              if (auto check_result =
                      fragment_instance
                          .CheckIfAnySocketIsWithinConnectionDistance();
                  check_result.has_value()) {
                return std::make_optional(
                    std::make_pair(part_id, check_result.value()));
              }
              return std::nullopt;
            },
            [&](const JointInstance &joint_instance)
                -> std::optional<std::pair<uint32_t, uint32_t>> {
              if (auto check_result =
                      joint_instance
                          .CheckIfAnySocketIsWithinConnectionDistance();
                  check_result.has_value()) {
                return std::make_optional(
                    std::make_pair(part_id, check_result.value()));
              }
              return std::nullopt;
            },
        },
        part_instance);

    if (result.has_value()) {
      return result;
    }
  }

  return std::nullopt;
}

} // namespace steamrot::logic::action::grimoire_machina
