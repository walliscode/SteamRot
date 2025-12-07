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
#include "subscriber_generated.h"
#include <memory>

namespace steamrot {
namespace subscriber_factory {

/////////////////////////////////////////////////
std::expected<Subscriber, FailInfo>
CreateSubscriber(const SubscriberFbs *subscriber_fbs) {
  
  // Validate input
  if (!subscriber_fbs) {
    return std::unexpected(FailInfo{FailMode::NullPointer,
                                    "SubscriberFbs pointer is null"});
  }

  // Skip if EventType is NONE
  if (subscriber_fbs->event_type_data() == EventType::EventType_NONE) {
    return std::unexpected(FailInfo{FailMode::InvalidValue,
                                    "EventType is NONE"});
  }

  Subscriber subscriber;
  subscriber.m_trigger_event_type = subscriber_fbs->event_type_data();
  subscriber.m_active = subscriber_fbs->active();

  // Check for trigger data
  if (subscriber_fbs->trigger_data()) {
    // Convert flatbuffers data to EventData
    auto convert_result =
        event::CreateEventData(subscriber_fbs->trigger_data_type(),
                               subscriber_fbs->trigger_data());
    if (!convert_result.has_value()) {
      return std::unexpected(convert_result.error());
    }
    subscriber.m_trigger_event_data = convert_result.value();
  }

  return subscriber;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CreateAndRegisterSubscribers(
    const std::vector<Subscriber> &subscribers_input,
    std::vector<std::shared_ptr<Subscriber>> &subscribers_output,
    EventHandler &event_handler) {

  // Iterate through each subscriber
  for (const auto &subscriber_input : subscribers_input) {

    // Skip if EventType is NONE
    if (subscriber_input.m_trigger_event_type == EventType::EventType_NONE)
      continue;

    // Create shared pointer to subscriber
    auto subscriber = std::make_shared<Subscriber>(subscriber_input);

    // Register subscriber with EventHandler
    auto result = event_handler.RegisterSubscriber(subscriber);
    if (!result.has_value())
      return std::unexpected(result.error());

    // Add subscriber to the provided vector
    subscribers_output.push_back(subscriber);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CreateAndRegisterSubscribers(
    const std::vector<const SubscriberFbs *> &subscribers_fbs,
    std::vector<std::shared_ptr<Subscriber>> &subscribers_output,
    EventHandler &event_handler) {

  // Iterate through each SubscriberFbs
  for (const auto *subscriber_fbs : subscribers_fbs) {

    // Skip null entries
    if (!subscriber_fbs)
      continue;

    // Convert SubscriberFbs to Subscriber
    auto convert_result = CreateSubscriber(subscriber_fbs);
    if (!convert_result.has_value()) {
      // Skip entries that fail to convert (e.g., NONE event types)
      if (convert_result.error().mode == FailMode::InvalidValue)
        continue;
      return std::unexpected(convert_result.error());
    }

    // Create shared pointer to subscriber
    auto subscriber = std::make_shared<Subscriber>(convert_result.value());

    // Register subscriber with EventHandler
    auto result = event_handler.RegisterSubscriber(subscriber);
    if (!result.has_value())
      return std::unexpected(result.error());

    // Add subscriber to the provided vector
    subscribers_output.push_back(subscriber);
  }

  return std::monostate{};
}

} // namespace subscriber_factory
} // namespace steamrot
