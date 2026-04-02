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
#include "MachinaFormScaffold.h"
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
/// @brief Change color of a GrowthPoint based on is_mouse_over
///
/// @param growth_point GrowthPoint reference
/////////////////////////////////////////////////
void SetColor(GrowthPoint &growth_point);

/////////////////////////////////////////////////
/// @brief Change color of a Socket based on is_mouse_over
///
/// @param socket Socket reference
/////////////////////////////////////////////////
void SetColor(Socket &socket);

/////////////////////////////////////////////////
/// @brief Change color of all sockets on a FragmentInstance based on
/// is_mouse_over
///
/// @param fragment_instance FragmentInstance reference
/////////////////////////////////////////////////
void SetColor(FragmentInstance &fragment_instance);

/////////////////////////////////////////////////
/// @brief Change color of all sockets on a JointInstance based on
/// is_mouse_over
///
/// @param joint_instance JointInstance reference
/////////////////////////////////////////////////
void SetColor(JointInstance &joint_instance);

/////////////////////////////////////////////////
/// @brief Return the string names of all fragments in the GrimoireMachina.
///
/// @param grimoire_machina Instance of GrimoireMachina to get the fragment
/// names from.
/////////////////////////////////////////////////
std::vector<std::string>
GetAllFragmentNames(const GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Add a fragment to the active MachinaFormScaffold.
///
/// Creates a FragmentInstance from the named fragment and appends it to the
/// scaffold's fragment list. If the scaffold does not yet exist it is
/// initialised first. Returns an error if the named fragment cannot be found.
///
/// @param grimoire_machina GrimoireMachina containing the fragment library and
///                         the active scaffold.
/// @param fragment_name    Name of the fragment to add.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AddFragmentToScaffold(GrimoireMachina &grimoire_machina,
                      const std::string &fragment_name);

} // namespace steamrot::logic::action::grimoire_machina
