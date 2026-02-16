/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Subscriber configuration functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_subscribers.h"
#include "EventData.h"
#include "event_factory.h"
#include "event_type_conversion.h"
#include "subscriber_generated.h"
#include <expected>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSubscriber(Subscriber &subscriber,
                    const SubscriberFbs *subscriber_fbs) {

  // Validate input
  if (!subscriber_fbs) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "SubscriberFbs pointer is null"});
  }

  // Set active state
  subscriber.m_active = subscriber_fbs->active();

  // Convert EventTypeFbs to native EventType
  auto event_type_result =
      event::ConvertEventTypeFbsToEventType(subscriber_fbs->event_type_data());
  if (!event_type_result.has_value()) {
    return std::unexpected(event_type_result.error());
  }
  subscriber.m_trigger_event_type = event_type_result.value();

  // Check for trigger data
  if (subscriber_fbs->trigger_data_type() != EventDataData_NONE &&
      subscriber_fbs->trigger_data()) {
    auto convert_result = event::CreateEventData(
        subscriber_fbs->trigger_data_type(), subscriber_fbs->trigger_data());
    if (!convert_result.has_value()) {
      return std::unexpected(convert_result.error());
    }
    subscriber.m_trigger_event_data = convert_result.value();
  }

  // Check for received data
  if (subscriber_fbs->received_data_type() != EventDataData_NONE &&
      subscriber_fbs->received_data()) {
    auto convert_result = event::CreateEventData(
        subscriber_fbs->received_data_type(), subscriber_fbs->received_data());
    if (!convert_result.has_value()) {
      return std::unexpected(convert_result.error());
    }
    subscriber.m_received_event_data = convert_result.value();
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<Subscriber, FailInfo>
CreateSubscriber(const SubscriberFbs *subscriber_fbs) {

  // Validate input
  if (!subscriber_fbs) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "SubscriberFbs pointer is null"});
  }

  // Skip if EventTypeFbs is NONE
  if (subscriber_fbs->event_type_data() == EventTypeFbs_NONE) {
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled, "EventTypeFbs is NONE"});
  }

  Subscriber subscriber;

  auto config_result = ConfigureSubscriber(subscriber, subscriber_fbs);
  if (!config_result.has_value()) {
    return std::unexpected(config_result.error());
  }

  return subscriber;
}

} // namespace steamrot::data::configure
