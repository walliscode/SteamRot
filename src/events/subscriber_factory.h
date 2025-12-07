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
#include "subscriber_generated.h"
#include <expected>
#include <memory>
#include <vector>

namespace steamrot {
namespace subscriber_factory {

/////////////////////////////////////////////////
/// @brief Convert a SubscriberFbs to a Subscriber object.
///
/// @param subscriber_fbs Pointer to the SubscriberFbs FlatBuffers object
/// @return Subscriber object or failure information
/////////////////////////////////////////////////
std::expected<Subscriber, FailInfo>
CreateSubscriber(const SubscriberFbs *subscriber_fbs);

/////////////////////////////////////////////////
/// @brief Create subscribers from a vector of Subscriber objects, register them
/// with EventHandler, and add them to the provided vector.
///
/// @param subscribers_input Vector of Subscriber objects to register.
/// @param subscribers_output Reference to vector for storing shared pointers to
/// registered subscribers.
/// @param event_handler Reference to the EventHandler for registering
/// subscribers.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CreateAndRegisterSubscribers(
    const std::vector<Subscriber> &subscribers_input,
    std::vector<std::shared_ptr<Subscriber>> &subscribers_output,
    EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Create subscribers from a vector of flatbuffers SubscriberFbs,
/// register them with EventHandler, and add them to the provided vector.
///
/// @param subscribers_fbs Vector of SubscriberFbs pointers to create
/// subscribers from.
/// @param subscribers_output Reference to vector for storing shared pointers to
/// created subscribers.
/// @param event_handler Reference to the EventHandler for registering
/// subscribers.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CreateAndRegisterSubscribers(
    const std::vector<const SubscriberFbs *> &subscribers_fbs,
    std::vector<std::shared_ptr<Subscriber>> &subscribers_output,
    EventHandler &event_handler);

} // namespace subscriber_factory
} // namespace steamrot
