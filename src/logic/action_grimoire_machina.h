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
/// @brief Dispatch a single active LOGIC subscriber to the correct
/// GrimoireMachina action free function.
///
/// Checks that the subscriber carries a LogicPayload, then switches on
/// LogicPayload::LogicToggle to call InitialiseActiveMachinaFormScaffold or
/// ClearActiveMachinaFormScaffold. Callers are responsible for checking that
/// @p subscriber is active before passing it in.
///
/// @param subscriber Active subscriber whose captured_payload holds the action.
/// @param grimoire_machina GrimoireMachina instance to mutate.
/////////////////////////////////////////////////
void ProcessSubscriber(Subscriber &subscriber,
                       GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Process all active LOGIC-type subscribers for scaffold init/clear
/// events.
///
/// Iterates the subscriber list, skips inactive entries, and dispatches each
/// active subscriber to @ref ProcessSubscriber.
///
/// @param subscribers Subscribers owned by the Logic instance.
/// @param grimoire_machina GrimoireMachina instance to mutate.
/////////////////////////////////////////////////
void ProcessScaffoldSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Process all active USER_INPUT TOGGLE_SOCKET_VISIBILITY-type
/// subscribers and toggle are_sockets_visible on the active scaffold.
///
/// When triggered, toggles MachinaFormScaffold::are_sockets_visible. Does
/// nothing if no active scaffold exists.
///
/// @param subscribers Subscribers owned by the Logic instance.
/// @param grimoire_machina GrimoireMachina instance that owns the scaffold.
/////////////////////////////////////////////////
void ProcessSocketVisibilitySubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Process all active USER_INPUT SELECT-type subscribers and, when all
/// placement guards pass, place the ghost item on the active scaffold.
///
/// Guards checked in order:
///  1. Ghost selection must not be monostate (an item must be selected).
///  2. The click must not land on any visible UI element.
///  3. An active scaffold must exist on the GrimoireMachina.
///  4. The mouse position must be inside the crafting canvas.
///
/// @param subscribers Subscribers owned by the Logic instance.
/// @param scene_context SceneContext providing archetypes, entities, ghost
/// state, camera, and mouse position.
/// @param grimoire_machina GrimoireMachina instance that owns the scaffold.
/////////////////////////////////////////////////
void ProcessPlacementSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    const SceneContext &scene_context, GrimoireMachina &grimoire_machina);

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
/// @brief Process all active subscribers in a single pass, dispatching each
/// to the appropriate GrimoireMachina action.
///
/// Iterates the subscriber list once, skipping inactive entries, then routes
/// each active subscriber by event type:
///  - LOGIC events are forwarded to @ref ProcessSubscriber for scaffold
///    init/clear handling.
///  - USER_INPUT TOGGLE_SOCKET_VISIBILITY events toggle socket visibility on
///    the active scaffold.
///  - USER_INPUT SELECT events run the full placement guard chain and, when
///    all guards pass, place the ghost item via @ref PlaceGhostOnScaffold.
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
