/////////////////////////////////////////////////
/// @file
/// @brief Implementation of GrimoireMachinaActionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaActionLogic.h"
#include "EventPayload.h"
#include "EventType.h"
#include "Subscriber.h"
#include "action_grimoire_machina.h"

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaActionLogic::GrimoireMachinaActionLogic(
    const SceneContext scene_context)
    : Logic(scene_context) {

  // Register a subscriber for SELECT_AND_PLACE PLACE_ITEM events so this
  // Logic can place fragments on the scaffold when the user drops an item.
  // Empty item_name and item_type act as wildcards - handle any item type.
  auto place_subscriber = std::make_shared<Subscriber>();
  place_subscriber->event_type = EventType::SELECT_AND_PLACE;
  place_subscriber->filter_payload = SelectAndPlacePayload{
      SelectAndPlacePayload::Action::PLACE_ITEM, "", ""};
  AddSubscriber(place_subscriber);
  m_scene_context.event_handler.RegisterSubscriber(place_subscriber);
}

/////////////////////////////////////////////////
void GrimoireMachinaActionLogic::ProcessLogic() {

  // get THE grimoire machina from the AssetManager
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();
  MachinaFormScaffold *active_scaffold_form =
      grimoire_machina.m_scaffold_form.get();

  // cycle through the subscribers. If active, process
  for (auto &subscriber : m_subscribers) {

    // set up empty variables for event type and data. If the subscriber is
    // active, these will be populated
    EventType event_type = EventType::NONE;

    // check if the subscriber is active.
    if (subscriber->m_active) {

      // set the event data and type for processing
      event_type = subscriber->event_type;

    } else {
      continue;
    }

    // add logic for processing the event type and data here.
    if (event_type == EventType::LOGIC &&
        std::holds_alternative<LogicPayload>(
            subscriber->captured_payload.value())) {

      LogicPayload &logic_payload =
          std::get<LogicPayload>(subscriber->captured_payload.value());

      if (logic_payload.toggle_name ==
          LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD) {
        auto initialise_result =
            action::grimoire_machina::InitialiseActiveMachinaFormScaffold(
                grimoire_machina);
      }

      if (logic_payload.toggle_name ==
          LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD) {

        auto clear_result =
            action::grimoire_machina::ClearActiveMachinaFormScaffold(
                grimoire_machina);
      }
    }

    // Handle SELECT_AND_PLACE PLACE_ITEM: add the named item to the scaffold
    if (event_type == EventType::SELECT_AND_PLACE &&
        subscriber->captured_payload.has_value() &&
        std::holds_alternative<SelectAndPlacePayload>(
            subscriber->captured_payload.value())) {

      const SelectAndPlacePayload &place_payload =
          std::get<SelectAndPlacePayload>(subscriber->captured_payload.value());

      if (place_payload.action == SelectAndPlacePayload::Action::PLACE_ITEM &&
          place_payload.item_type == "fragment") {
        action::grimoire_machina::AddFragmentToScaffold(grimoire_machina,
                                                        place_payload.item_name);
      }
    }
  }

  // run action logic based on their being an active MachinFormScaffold
  if (active_scaffold_form)
    action::grimoire_machina::SetColor(active_scaffold_form->growth_point);
}
} // namespace steamrot::logic
