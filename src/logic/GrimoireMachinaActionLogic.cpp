/////////////////////////////////////////////////
/// @file
/// @brief Implementation of GrimoireMachinaActionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaActionLogic.h"
#include "EventType.h"

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

  // cycle through the subscribers. If active, process
  for (auto &subscriber : m_subscribers) {

    // set up empty variables for event type and data. If the subscriber is
    // active, these will be populated
    EventType event_type = EventType::NONE;
    EventData event_data = std::monostate{};

    // check if the subscriber is active.
    if (subscriber->m_active) {

      // set the event data and type for processing
      event_type = subscriber->m_trigger_event_type;
      if (subscriber->m_trigger_event_data.has_value()) {
        event_data = subscriber->m_trigger_event_data.value();
      }

    } else {
      continue;
    }
  }
}
} // namespace steamrot::logic
