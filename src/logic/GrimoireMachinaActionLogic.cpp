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
#include "action_grimoire_machina.h"

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaActionLogic::GrimoireMachinaActionLogic(
    const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GrimoireMachinaActionLogic::ProcessLogic() {

  // get THE grimoire machina from the AssetManager
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
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
            actions::grimoire_machina::InitialiseActiveMachinaFormScaffold(
                grimoire_machina);
      }

      if (logic_payload.toggle_name ==
          LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD) {
        auto clear_result =
            actions::grimoire_machina::ClearActiveMachinaFormScaffold(
                grimoire_machina);
      }
    }
  }

  // run action logic based on their being an active MachinFormScaffold
  if (active_scaffold_form)
    actions::grimoire_machina::SetGrowthPointColor(
        active_scaffold_form->growth_point);
}
} // namespace steamrot::logic
