/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for processing actions related to
/// MrGhost.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayload.h"
#include "FailInfo.h"
#include "MrGhost.h"
#include <expected>

namespace steamrot::logic::action::ghost {

/////////////////////////////////////////////////
/// @brief Attach a GhostPayload to MrGhost, updating its selection state.
///
/// Applies the action carried by @p payload to @p mr_ghost:
///   - SELECT → sets mr_ghost.m_selection to payload.m_selection.
///   - CLEAR / NONE → resets mr_ghost.m_selection to std::monostate.
///
/// @param mr_ghost MrGhost instance to update.
/// @param payload  GhostPayload describing the change to apply.
/// @return std::monostate on success.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AttachGhostPayload(MrGhost &mr_ghost, const GhostPayload &payload);

/////////////////////////////////////////////////
/// @brief Clear the current GhostSelection on MrGhost.
///
/// Sets mr_ghost.m_selection to std::monostate, indicating no active
/// selection.
///
/// @param mr_ghost MrGhost instance whose selection will be cleared.
/////////////////////////////////////////////////
void ClearGhostSelection(MrGhost &mr_ghost);

} // namespace steamrot::logic::action::ghost
