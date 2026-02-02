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
#include "event_type_conversion.h"
#include "events_generated.h"
#include "subscriber_generated.h"
#include <expected>

namespace steamrot {
namespace subscriber_factory {

/////////////////////////////////////////////////
std::expected<Subscriber, FailInfo>
CreateSubscriber(const SubscriberFbs *subscriber_fbs) {

  // Validate input
  if (!subscriber_fbs) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "SubscriberFbs pointer is null"});
  }

  // Skip if EventTypeFbs is NONE
  if (subscriber_fbs->event_type_data() == EventTypeFbs_EVENT_NONE) {
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled, "EventTypeFbs is NONE"});
  }

  Subscriber subscriber;

  // Convert EventTypeFbs to native EventType
  auto event_type_result =
      event::ConvertEventTypeFbsToEventType(subscriber_fbs->event_type_data());
  if (!event_type_result.has_value()) {
    return std::unexpected(event_type_result.error());
  }
  subscriber.m_trigger_event_type = event_type_result.value();
  subscriber.m_active = subscriber_fbs->active();

  // Check for trigger data
  if (subscriber_fbs->trigger_data()) {
    // Convert flatbuffers data to EventData
    auto convert_result = event::CreateEventData(
        subscriber_fbs->trigger_data_type(), subscriber_fbs->trigger_data());
    if (!convert_result.has_value()) {
      return std::unexpected(convert_result.error());
    }
    subscriber.m_trigger_event_data = convert_result.value();
  }

  return subscriber;
}

} // namespace subscriber_factory
} // namespace steamrot
