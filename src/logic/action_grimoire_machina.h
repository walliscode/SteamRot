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
#include "FailInfo.h"
#include "GrimoireMachina.h"
#include "MrGhost.h"
#include "SceneContext.h"
#include "Subscriber.h"
#include <SFML/System/Vector2.hpp>
#include <expected>
#include <memory>
#include <variant>
#include <vector>

namespace steamrot::logic::action::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Intialise the MachinaFormScaffold in the GrimoireMachine.
///
/// @param grimoire_machina GrimoireMachina to intialise the active MachinaForm
/// in.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InitialiseActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Clear the active ClearActiveMachinaFormScaffold in the
/// GrimoireMachina.
///
/// @param grimoire_machina GrimoireMachina to clear the active MachinaForm in.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ClearActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina);

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
/// @brief Handle a single USER_INPUT TOGGLE_SOCKET_VISIBILITY subscriber and
/// toggle are_sockets_visible on the active scaffold.
///
/// When triggered, toggles MachinaFormScaffold::are_sockets_visible. Does
/// nothing if the payload is not TOGGLE_SOCKET_VISIBILITY or no active
/// scaffold exists.
///
/// @param subscriber    Subscriber to process.
/// @param grimoire_machina GrimoireMachina instance that owns the scaffold.
/////////////////////////////////////////////////
void ProcessSocketVisibilitySubscribers(Subscriber &subscriber,
                                        GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Handle a single USER_INPUT SELECT subscriber and, when all placement
/// guards pass, place the ghost item on the active scaffold.
///
/// Guards checked in order:
///  1. Ghost selection must not be monostate (an item must be selected).
///  2. SceneState::is_mouse_over_ui_layer must be false (no UI hover).
///  3. An active scaffold must exist on the GrimoireMachina.
///
/// @param subscriber    Subscriber to process.
/// @param scene_context SceneContext providing archetypes, entities, ghost
/// state, camera, and mouse position.
/// @param grimoire_machina GrimoireMachina instance that owns the scaffold.
/////////////////////////////////////////////////
void ProcessPlacementSubscribers(Subscriber &subscriber,
                                 const SceneContext &scene_context,
                                 GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Handle a single USER_INPUT subscriber by routing to
/// @ref ProcessSocketVisibilitySubscribers and @ref ProcessPlacementSubscribers.
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
/// Looks up the Fragment or Joint identified by @p mr_ghost's selection tag,
/// builds an instance with a transform that centers it on @p world_pos, and
/// appends it to the scaffold.  Returns an error if no scaffold is active,
/// the scaffold already contains pieces (not the first piece), the selection
/// is empty (std::monostate), or the key is not found in the GrimoireMachina's
/// library.
///
/// @param grimoire_machina GrimoireMachina that owns the active scaffold.
/// @param mr_ghost         Current ghost state providing the selection tag.
/// @param world_pos        World-space position at which to center the piece.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
PlaceFirstPiece(GrimoireMachina &grimoire_machina, const MrGhost &mr_ghost,
                sf::Vector2f world_pos);

/////////////////////////////////////////////////
/// @brief Place the currently selected ghost item onto the active scaffold.
///
/// Routes to @ref PlaceFirstPiece when the scaffold is empty.  For subsequent
/// pieces a positive socket-proximity collision result is required; because
/// that logic is not yet implemented this path always returns an error.
///
/// Returns an error if no scaffold is active, the selection is empty
/// (std::monostate), or the key is not found in the GrimoireMachina's library.
///
/// @param grimoire_machina GrimoireMachina that owns the active scaffold.
/// @param mr_ghost         Current ghost state providing the selection tag.
/// @param world_pos        World-space position at which to place the part.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
PlaceGhostOnScaffold(GrimoireMachina &grimoire_machina, const MrGhost &mr_ghost,
                     sf::Vector2f world_pos);

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
