/////////////////////////////////////////////////
/// @file
/// @brief Implementation of SubscriberDataViewer.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberDataViewer.h"
#include "event_factory.h"

namespace steamrot {

/////////////////////////////////////////////////
SubscriberDataViewer::SubscriberDataViewer(
    const flatbuffers::Vector<flatbuffers::Offset<SubscriberConfigFbs>>
        *subscriber_configs_fbs)
    : m_subscriber_configs_fbs(subscriber_configs_fbs) {}

/////////////////////////////////////////////////
std::expected<std::vector<SubscriberConfig>, FailInfo>
SubscriberDataViewer::GetSubscriberConfigs() const {
  std::vector<SubscriberConfig> configs;

  // Handle null case
  if (!m_subscriber_configs_fbs) {
    return configs; // Return empty vector if no data
  }

  // Convert each FlatBuffers SubscriberConfigFbs to SubscriberConfig
  for (const auto *subscriber_config_fbs : *m_subscriber_configs_fbs) {
    if (!subscriber_config_fbs) {
      continue; // Skip null entries
    }

    SubscriberConfig config;

    // Set event type
    config.trigger_event_type = subscriber_config_fbs->event_type_data();

    // Set active flag
    config.active = subscriber_config_fbs->active();

    // Convert trigger data if present
    if (subscriber_config_fbs->trigger_data()) {
      auto convert_result =
          event::CreateEventData(subscriber_config_fbs->trigger_data_type(),
                                 subscriber_config_fbs->trigger_data());
      if (!convert_result.has_value()) {
        return std::unexpected(convert_result.error());
      }
      config.trigger_event_data = convert_result.value();
    }

    configs.push_back(config);
  }

  return configs;
}

} // namespace steamrot
