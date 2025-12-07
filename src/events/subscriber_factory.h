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
#include <vector>

namespace steamrot {
namespace subscriber_factory {

/////////////////////////////////////////////////
/// @brief Create subscribers from a vector of configs, register them with
/// EventHandler, and add them to the provided vector.
///
/// @param configs Vector of SubscriberConfig to create subscribers from.
/// @param subscribers Reference to vector for storing created subscribers.
/// @param event_handler Reference to the EventHandler for registering
/// subscribers.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CreateAndRegisterSubscribers(
    const std::vector<SubscriberConfig> &configs,
    std::vector<std::shared_ptr<Subscriber>> &subscribers,
    EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Create subscribers from a vector of flatbuffers configs, register
/// them with EventHandler, and add them to the provided vector.
///
/// @param configs Vector of SubscriberConfigFbs pointers to create subscribers
/// from.
/// @param subscribers Reference to vector for storing created subscribers.
/// @param event_handler Reference to the EventHandler for registering
/// subscribers.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CreateAndRegisterSubscribers(
    const std::vector<const SubscriberConfigFbs *> &configs,
    std::vector<std::shared_ptr<Subscriber>> &subscribers,
    EventHandler &event_handler);

} // namespace subscriber_factory
} // namespace steamrot
