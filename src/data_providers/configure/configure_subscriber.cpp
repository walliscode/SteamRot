/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for configuring subscriber based
/// data
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_subscriber.h"
#include "FailInfo.h"
#include "configure_event.h"
#include <expected>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSubscriber(Subscriber &subscriber,
                    const SubscriberFbs *fb_subscriber) {

  // check for null data
  if (fb_subscriber == nullptr) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "SubscriberFbs data is null, cannot populate Subscriber"});
  }

  // set active to false by default,
  subscriber.m_active = false;

  // configure event type
  auto event_type_result =
      ConfigureEventType(subscriber.event_type, fb_subscriber->event_type());
  if (!event_type_result.has_value()) {
    return std::unexpected(event_type_result.error());
  }

  // configure filter payload if it exists
  if (fb_subscriber->filter_payload_type() != EventPayloadFbs_NONE) {

    // call configure free function
    auto filter_payload_result = ConfigureEventPayload(
        subscriber.filter_payload, fb_subscriber->filter_payload_type(),
        fb_subscriber->filter_payload());
    if (!filter_payload_result.has_value()) {
      return std::unexpected(filter_payload_result.error());
    }
  }

  // finish function
  return std::monostate();
}
} // namespace steamrot::data::configure
