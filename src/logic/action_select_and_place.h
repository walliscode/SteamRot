/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for the select-and-place system.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "EventPayload.h"
#include "SelectAndPlaceState.h"
#include <string>

namespace steamrot::logic::action::select_and_place {

/////////////////////////////////////////////////
/// @brief Record that an item has been selected for placement.
///
/// Sets the is_item_selected flag and stores the item name and type in state.
///
/// @param state      State to update
/// @param item_name  Name of the selected item
/// @param item_type  Category of the selected item (e.g. "fragment", "joint")
/////////////////////////////////////////////////
void SelectItem(SelectAndPlaceState &state, const std::string &item_name,
                const std::string &item_type);

/////////////////////////////////////////////////
/// @brief Fire a PLACE_ITEM event for the currently selected item.
///
/// Adds a SelectAndPlacePayload{PLACE_ITEM} event to the EventHandler's
/// waiting room. Does nothing if no item is currently selected.
///
/// @param state         Current selection state
/// @param event_handler EventHandler to receive the placement event
/////////////////////////////////////////////////
void PlaceItem(const SelectAndPlaceState &state, EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Clear the current item selection, returning to idle state.
///
/// @param state State to clear
/////////////////////////////////////////////////
void ClearSelection(SelectAndPlaceState &state);

} // namespace steamrot::logic::action::select_and_place
