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
void InitialiseActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Clear the active ClearActiveMachinaFormScaffold in the
/// GrimoireMachina.
///
/// @param grimoire_machina GrimoireMachina to clear the active MachinaForm in.
/////////////////////////////////////////////////
void ClearActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Return the string names of all fragments in the GrimoireMachina.
///
/// @param grimoire_machina Instance of GrimoireMachina to get the fragment
/// names from.
/////////////////////////////////////////////////
std::vector<std::string> GetAllFragmentNames(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Return the string names of all joints in the GrimoireMachina.
///
/// @param grimoire_machina Instance of GrimoireMachina to get the joint
/// names from.
/////////////////////////////////////////////////
std::vector<std::string> GetAllJointNames(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Toggle the are_sockets_visible boolean on the active
/// MachinaFormScaffold in the GrimoireMachina.
///
/// @param scaffold MachinaFormScaffold to toggle the socket visibility on.
/////////////////////////////////////////////////
void ToggleSocketVisibility(MachinaFormScaffold &scaffold);

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
void ProcessLogicEvents(Subscriber &subscriber,
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
void ProcessUserInputEvents(Subscriber &subscriber,
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
void PlaceFirstPiece(GrimoireMachina &grimoire_machina,
                     const MrGhost &mr_ghost);

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
void PlaceGhostOnScaffold(GrimoireMachina &grimoire_machina,
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
void ProcessSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    const SceneContext &scene_context, GrimoireMachina &grimoire_machina);

} // namespace steamrot::logic::action::grimoire_machina
