/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for the select-and-place system.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_select_and_place.h"
#include "event_factory.h"

namespace steamrot::logic::action::select_and_place {

/////////////////////////////////////////////////
void SelectItem(SelectAndPlaceState &state, const std::string &item_name,
                const std::string &item_type) {
  state.is_item_selected = true;
  state.selected_item_name = item_name;
  state.selected_item_type = item_type;
}

/////////////////////////////////////////////////
void PlaceItem(const SelectAndPlaceState &state, EventHandler &event_handler) {
  if (!state.is_item_selected) {
    return;
  }

  auto packet_result = events::CreateSelectAndPlaceEventPacket(
      1, SelectAndPlacePayload::Action::PLACE_ITEM, state.selected_item_name,
      state.selected_item_type);

  if (packet_result.has_value()) {
    event_handler.AddEvent(packet_result.value());
  }
}

/////////////////////////////////////////////////
void ClearSelection(SelectAndPlaceState &state) { state.Clear(); }

} // namespace steamrot::logic::action::select_and_place
