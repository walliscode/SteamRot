/////////////////////////////////////////////////
/// @file
/// @brief Definition of the subscriber_factory namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "FailInfo.h"
#include "Subscriber.h"
#include "SubscriberConfig.h"
#include "subscriber_config_generated.h"
#include <expected>
#include <memory>

namespace steamrot {
namespace subscriber_factory {

/////////////////////////////////////////////////
/// @brief Create a Subscriber from SubscriberConfig and register it with
/// EventHandler.
///
/// @param config SubscriberConfig to create the subscriber from.
/// @param event_handler Reference to the EventHandler for registering
/// subscribers.
/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
CreateAndRegisterSubscriber(const SubscriberConfig &config,
                            EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Create a Subscriber from flatbuffers data and register it with
/// EventHandler.
///
/// @param subscriber_config_fbs Flatbuffers SubscriberConfigFbs to create the
/// subscriber from.
/// @param event_handler Reference to the EventHandler for registering
/// subscribers.
/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
CreateAndRegisterSubscriber(const SubscriberConfigFbs &subscriber_config_fbs,
                            EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Given event type, create and register a subscriber.
///
/// @param event_type Reference to the EventType for the subscriber
/// @param event_handler Reference to the EventHandler for registering
/// subscribers.
/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
CreateAndRegisterSubscriber(const EventType &event_type,
                            EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Given event type and trigger data, create and register a
/// subscriber.
///
/// @param event_type Reference to the EventType for the subscriber
/// @param trigger_data Reference to the EventData for the subscriber trigger
/// @param event_handler Reference to the EventHandler for registering
/// subscribers.
/////////////////////////////////////////////////
std::expected<std::shared_ptr<Subscriber>, FailInfo>
CreateAndRegisterSubscriber(const EventType &event_type,
                            const EventData &trigger_data,
                            EventHandler &event_handler);

} // namespace subscriber_factory
} // namespace steamrot
