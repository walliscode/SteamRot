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
#include "MrGhost.h"

namespace steamrot::logic::action::ghost {

/////////////////////////////////////////////////
/// @brief Apply a ghost selection to MrGhost.
///
/// Sets mr_ghost.m_selection to the provided @p selection value.
/// Callers are responsible for choosing this function when the relevant
/// GhostPayload::GhostAction is SELECT.
///
/// @param mr_ghost  MrGhost instance to update.
/// @param selection GhostSelection variant describing the chosen item.
/////////////////////////////////////////////////
void SelectGhostItem(MrGhost &mr_ghost, const GhostSelection &selection);

/////////////////////////////////////////////////
/// @brief Clear the current GhostSelection on MrGhost.
///
/// Sets mr_ghost.m_selection to std::monostate, indicating no active
/// selection. Callers are responsible for choosing this function when
/// the relevant GhostPayload::GhostAction is CLEAR or NONE.
///
/// @param mr_ghost MrGhost instance whose selection will be cleared.
/////////////////////////////////////////////////
void ClearGhostSelection(MrGhost &mr_ghost);

} // namespace steamrot::logic::action::ghost
