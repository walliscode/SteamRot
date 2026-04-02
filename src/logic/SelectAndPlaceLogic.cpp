/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SelectAndPlaceLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SelectAndPlaceLogic.h"
#include "EventPayload.h"
#include "EventType.h"
#include "Subscriber.h"
#include "action_select_and_place.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
SelectAndPlaceLogic::SelectAndPlaceLogic(const SceneContext scene_context)
    : Logic(scene_context) {

  // --- Subscriber for SELECT_ITEM events ---
  // Activated whenever a SELECT_AND_PLACE event with action SELECT_ITEM
  // arrives on the bus. The empty item_name/item_type act as wildcards.
  auto select_subscriber = std::make_shared<Subscriber>();
  select_subscriber->event_type = EventType::SELECT_AND_PLACE;
  select_subscriber->filter_payload = SelectAndPlacePayload{
      SelectAndPlacePayload::Action::SELECT_ITEM, "", ""};
  AddSubscriber(select_subscriber);
  m_scene_context.event_handler.RegisterSubscriber(select_subscriber);

  // --- Subscriber for USER_INPUT SELECT events (placement trigger) ---
  // When the user performs a SELECT input and an item is already selected,
  // this Logic will fire a PLACE_ITEM event.
  auto place_trigger_subscriber = std::make_shared<Subscriber>();
  place_trigger_subscriber->event_type = EventType::USER_INPUT;
  place_trigger_subscriber->filter_payload =
      InputPayload{InputPayload::InputAction::SELECT};
  AddSubscriber(place_trigger_subscriber);
  m_scene_context.event_handler.RegisterSubscriber(place_trigger_subscriber);
}

/////////////////////////////////////////////////
void SelectAndPlaceLogic::ProcessLogic() {

  for (auto &subscriber : m_subscribers) {

    if (!subscriber->m_active) {
      continue;
    }

    if (!subscriber->captured_payload.has_value()) {
      continue;
    }

    // Handle SELECT_ITEM: store the selected item in state
    if (subscriber->event_type == EventType::SELECT_AND_PLACE &&
        std::holds_alternative<SelectAndPlacePayload>(
            subscriber->captured_payload.value())) {

      const SelectAndPlacePayload &payload =
          std::get<SelectAndPlacePayload>(subscriber->captured_payload.value());

      if (payload.action == SelectAndPlacePayload::Action::SELECT_ITEM) {
        action::select_and_place::SelectItem(m_select_and_place_state,
                                             payload.item_name, payload.item_type);
      }
    }

    // Handle USER_INPUT SELECT: if an item is selected, fire PLACE_ITEM
    else if (subscriber->event_type == EventType::USER_INPUT &&
             m_select_and_place_state.is_item_selected &&
             std::holds_alternative<InputPayload>(
                 subscriber->captured_payload.value())) {

      const InputPayload &input_payload =
          std::get<InputPayload>(subscriber->captured_payload.value());

      if (input_payload.action == InputPayload::InputAction::SELECT) {
        action::select_and_place::PlaceItem(m_select_and_place_state,
                                            m_scene_context.event_handler);
        action::select_and_place::ClearSelection(m_select_and_place_state);
      }
    }
  }
}

} // namespace steamrot::logic
