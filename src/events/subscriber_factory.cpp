/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the subscriber_factory namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "subscriber_factory.h"
#include "Subscriber.h"
#include "event_factory.h"
#include "subscriber_config_generated.h"
#include <memory>

namespace steamrot {
namespace subscriber_factory {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CreateAndRegisterSubscribers(
    const std::vector<SubscriberConfig> &configs,
    std::vector<std::shared_ptr<Subscriber>> &subscribers,
    EventHandler &event_handler) {

  // Iterate through each config
  for (const auto &config : configs) {

    // Skip if EventType is NONE
    if (config.trigger_event_type == EventType::EventType_NONE)
      continue;

    // Create subscriber based on config
    std::shared_ptr<Subscriber> subscriber{nullptr};

    if (config.trigger_event_data.has_value()) {
      subscriber = std::make_shared<Subscriber>(config.trigger_event_type,
                                                 config.trigger_event_data.value());
    } else {
      subscriber = std::make_shared<Subscriber>(config.trigger_event_type);
    }

    // Set active state if needed
    if (config.active) {
      subscriber->m_active = true;
    }

    // Register subscriber with EventHandler
    auto result = event_handler.RegisterSubscriber(subscriber);
    if (!result.has_value())
      return std::unexpected(result.error());

    // Add subscriber to the provided vector
    subscribers.push_back(subscriber);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CreateAndRegisterSubscribers(
    const std::vector<const SubscriberConfigFbs *> &configs,
    std::vector<std::shared_ptr<Subscriber>> &subscribers,
    EventHandler &event_handler) {

  // Iterate through each config
  for (const auto *subscriber_config_fbs : configs) {

    // Skip null configs
    if (!subscriber_config_fbs)
      continue;

    // Skip if EventType is NONE
    if (subscriber_config_fbs->event_type_data() == EventType::EventType_NONE)
      continue;

    // Create subscriber based on config
    std::shared_ptr<Subscriber> subscriber{nullptr};
    EventType event_type = subscriber_config_fbs->event_type_data();

    // Check for trigger data
    if (subscriber_config_fbs->trigger_data()) {
      // Convert flatbuffers data to EventData
      auto convert_result =
          event::CreateEventData(subscriber_config_fbs->trigger_data_type(),
                                 subscriber_config_fbs->trigger_data());
      if (!convert_result.has_value()) {
        return std::unexpected(convert_result.error());
      }
      EventData trigger_data = convert_result.value();
      subscriber = std::make_shared<Subscriber>(event_type, trigger_data);
    } else {
      subscriber = std::make_shared<Subscriber>(event_type);
    }

    // Set active state if needed
    if (subscriber_config_fbs->active()) {
      subscriber->m_active = true;
    }

    // Register subscriber with EventHandler
    auto result = event_handler.RegisterSubscriber(subscriber);
    if (!result.has_value())
      return std::unexpected(result.error());

    // Add subscriber to the provided vector
    subscribers.push_back(subscriber);
  }

  return std::monostate{};
}

} // namespace subscriber_factory
} // namespace steamrot
