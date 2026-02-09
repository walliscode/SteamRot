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
#include <expected>
#include <variant>

namespace steamrot::logic::actions::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Intialise the active MachinaForm in the GrimoireMachina.
///
/// @param grimoire_machina GrimoireMachina to intialise the active MachinaForm
/// in.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InitialiseActiveMachinaForm(GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Clear the active MachinaForm in the GrimoireMachina.
///
/// @param grimoire_machina GrimoireMachina to clear the active MachinaForm in.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ClearActiveMachinaForm(GrimoireMachina &grimoire_machina);
} // namespace steamrot::logic::actions::grimoire_machina
