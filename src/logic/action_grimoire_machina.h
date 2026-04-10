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
#include "Subscriber.h"
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

} // namespace steamrot::logic::action::grimoire_machina
