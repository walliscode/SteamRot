/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for process actions related to the
/// GrimoireMachina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachina.h"
#include "MachinaFormScaffold.h"
#include "MrGhost.h"
#include "SceneContext.h"
#include "SocketState.h"
#include "Subscriber.h"
#include <memory>
#include <vector>

namespace steamrot::logic::action::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Intialise the MachinaFormScaffold in the GrimoireMachine.
///
/// @param grimoire_machina GrimoireMachina to intialise the active MachinaForm
/// in.
/////////////////////////////////////////////////
void initialise_active_machina_form_scaffold(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Clear the active ClearActiveMachinaFormScaffold in the
/// GrimoireMachina.
///
/// @param grimoire_machina GrimoireMachina to clear the active MachinaForm in.
/////////////////////////////////////////////////
void clear_active_machina_form_scaffold(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Return the string names of all fragments in the GrimoireMachina.
///
/// @param grimoire_machina Instance of GrimoireMachina to get the fragment
/// names from.
/////////////////////////////////////////////////
std::vector<std::string>
get_all_fragment_names(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Return the string names of all joints in the GrimoireMachina.
///
/// @param grimoire_machina Instance of GrimoireMachina to get the joint
/// names from.
/////////////////////////////////////////////////
std::vector<std::string> get_all_joint_names(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Toggle the are_sockets_visible boolean on the active
/// MachinaFormScaffold in the GrimoireMachina.
///
/// @param scaffold MachinaFormScaffold to toggle the socket visibility on.
/////////////////////////////////////////////////
void toggle_socket_visibility(MachinaFormScaffold &scaffold);

/////////////////////////////////////////////////
/// @brief Handle a single LOGIC subscriber for scaffold init/clear events.
///
/// Checks that the subscriber carries a LogicPayload, then switches on
/// LogicPayload::LogicToggle to call InitialiseActiveMachinaFormScaffold or
/// ClearActiveMachinaFormScaffold.
///
/// @param subscriber Subscriber whose captured_payload holds the action.
/// @param grimoire_machina GrimoireMachina instance to mutate.
/////////////////////////////////////////////////
void process_logic_events(Subscriber &subscriber,
                          GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Handle a single USER_INPUT SELECT subscriber and, when all placement
/// guards pass, place the ghost item on the active scaffold.
///
/// @param subscriber    Subscriber to process.
/// @param scene_context SceneContext providing archetypes, entities, ghost
/// state, camera, and mouse position.
/// @param grimoire_machina GrimoireMachina instance to mutate.
/////////////////////////////////////////////////
void proces_user_input_events(Subscriber &subscriber,
                              const SceneContext &scene_context,
                              GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Place the currently selected ghost item as the very first piece on
/// the active scaffold.
///
/// Looks up the Fragment or Joint identified by @p mr_ghost's selection tag
/// and appends a default-transform instance to the scaffold. Does nothing if
/// no scaffold is active, the scaffold already contains pieces, the selection
/// is monostate, or the key is not found.
///
/// @param grimoire_machina GrimoireMachina that owns the active scaffold.
/// @param mr_ghost         Current ghost state providing the selection tag.
/////////////////////////////////////////////////
void place_first_piece(GrimoireMachina &grimoire_machina,
                       const MrGhost &mr_ghost);

/////////////////////////////////////////////////
/// @brief Place the current ghost item onto the scaffold as the next piece,
/// creating a connection when both the ghost and the scaffold have sockets
/// ready to connect.
///
/// Calls @ref check_MrGhost_for_connection_readiness and
/// @ref check_PartGraph_for_connection_readiness. If both return a value the
/// ghost instance is copied into @p scaffold (with a new stable ID) and the
/// connection is recorded directly on both sockets via
/// @ref create_connection (setting @c SocketData::connected_to on each end).
///
/// By convention connections are only between a FragmentInstance and a
/// JointInstance. Does nothing if the ghost and the PartGraph part have the
/// same type (both Fragment or both Joint).
///
/// Does nothing if:
///   - @p scaffold.parts is empty (use @ref place_first_piece instead),
///   - the ghost instance is monostate or its part pointer is null,
///   - either readiness check returns @c std::nullopt, or
///   - the ghost type and the ready PartGraph part type are the same.
///
/// @param scaffold  The active MachinaFormScaffold to place the piece on.
/// @param mr_ghost  Current ghost state carrying the instance and its
///                  transform.
/////////////////////////////////////////////////
void place_next_piece(MachinaFormScaffold &scaffold, const MrGhost &mr_ghost);

/////////////////////////////////////////////////
/// @brief Place the currently selected ghost item onto the active scaffold.
///
/// Routes to @ref PlaceFirstPiece when the scaffold is empty. For subsequent
/// pieces a positive socket-proximity collision result is required; because
/// that logic is not yet implemented this path does nothing.
///
/// Does nothing if no scaffold is active.
///
/// @param grimoire_machina GrimoireMachina that owns the active scaffold.
/// @param mr_ghost         Current ghost state providing the selection tag.
/////////////////////////////////////////////////
void place_ghost_on_scaffold(GrimoireMachina &grimoire_machina,
                             const MrGhost &mr_ghost);

/////////////////////////////////////////////////
/// @brief Process all active subscribers by iterating the list, checking
/// active state and event type, then dispatching each subscriber to the
/// appropriate event-type handler.
///
/// For each active subscriber:
///  - LOGIC event type → @ref ProcessLogicEvents
///  - USER_INPUT event type → @ref ProcessUserInputEvents
///
/// @param subscribers    Subscribers owned by the Logic instance.
/// @param scene_context  SceneContext providing ghost state, archetypes,
///                       entities, camera, and mouse position.
/// @param grimoire_machina GrimoireMachina instance to mutate.
/////////////////////////////////////////////////
void process_subscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    const SceneContext &scene_context, GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Creates a connection between specific SocketData on two
/// PartInstances by setting @c SocketData::connected_to on both ends
/// symmetrically and marking both sockets as @c SocketState::Connected.
/// Returns an error string if validation fails.
///
/// Connections are, by convention, only between a JointInstance and a
/// FragmentInstance.
/// @param fragment FragmentInstance to connect.
/// @param socket_id_a Stable socket ID of the FragmentInstance to connect.
/// @param joint JointInstance to connect.
/// @param socket_id_b Stable socket ID of the JointInstance to connect.
/// @return @c std::monostate on success, or an error string if either part
/// has no sockets or a socket ID is not found.
/////////////////////////////////////////////////
std::expected<std::monostate, std::string>
create_connection(FragmentInstance &fragment_instance, uint32_t socket_id_a,
                  JointInstance &joint_instance, uint32_t socket_id_b);

/////////////////////////////////////////////////
/// @brief Checks whether the given SocketData is in a state that allows it to
/// be connected to another socket.
///
/// @param socket SocketData to check for connection readiness.
/// @return Boolean indicating whether the socket is ready to be connected
/// (true) or not (false).
/////////////////////////////////////////////////
bool check_socket_for_connection_readiness(const SocketState &socket);

/////////////////////////////////////////////////
/// @brief Checks whether any of the selections on MrGhost are ready to be
/// connected to the active MachinaFormScaffold
///
/// @param mr_ghost MrGhost instance living on the Scene
/////////////////////////////////////////////////
std::optional<uint32_t>
check_MrGhost_for_connection_readiness(const MrGhost &mr_ghost);

/////////////////////////////////////////////////
/// @brief Checks whether any of the sockets on the active MachinaFormScaffold
/// are ready to be connected to. Returns the part ID and socket index of the
/// first ready socket, or std::nullopt if no sockets are ready.
///
///
/// @param part_graph PartGraph to check
/////////////////////////////////////////////////
std::optional<std::pair<uint32_t, uint32_t>>
check_PartGraph_for_connection_readiness(const PartGraph &part_graph);

/////////////////////////////////////////////////
/// @brief Checks wether the given JointInstance and FragmentInstance have any
/// sockets that are connected. If so, returns a ConnectedSockets object
/// containing the socket connections. If not, returns std::nullopt.
///
/// @param joint_instance JointInstance to check for connected sockets
/// @param fragment_instance FragmentInstance to check for connected sockets
/////////////////////////////////////////////////
std::optional<JointFragmentConnection>
check_for_connected_sockets(const JointInstance &joint_instance,
                            const FragmentInstance &fragment_instance);
} // namespace steamrot::logic::action::grimoire_machina
