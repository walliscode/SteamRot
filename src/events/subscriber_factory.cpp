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

  // Skip if EventType is NONE
  if (subscriber_fbs->event_type_data() == EventType::EventType_NONE) {
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled, "EventType is NONE"});
  }

  Subscriber subscriber;
  subscriber.m_trigger_event_type = subscriber_fbs->event_type_data();
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
