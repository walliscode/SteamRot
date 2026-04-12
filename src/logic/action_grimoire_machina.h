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
#include "Subscriber.h"
#include <SFML/System/Vector2.hpp>
#include <expected>
#include <variant>

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
/// @brief Place the currently selected ghost item onto the active scaffold.
///
/// Looks up the Fragment or Joint identified by @p mr_ghost's selection
/// tag, builds an instance with a transform that places it at @p world_pos,
/// assigns it the next stable ID from the scaffold, and appends it to the
/// appropriate instance list.  Returns an error if no scaffold is active,
/// the selection is empty (std::monostate), or the key is not found in
/// the GrimoireMachina's library.
///
/// @param grimoire_machina GrimoireMachina that owns the active scaffold.
/// @param mr_ghost         Current ghost state providing the selection tag.
/// @param world_pos        World-space position at which to place the part.
/// @param is_first_piece   When true the part is centered on @p world_pos;
///                         when false the ghost anchor (bottom-right corner)
///                         is used so the piece visually snaps to the cursor.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
PlaceGhostOnScaffold(GrimoireMachina &grimoire_machina, const MrGhost &mr_ghost,
                     sf::Vector2f world_pos, bool is_first_piece = false);

} // namespace steamrot::logic::action::grimoire_machina
