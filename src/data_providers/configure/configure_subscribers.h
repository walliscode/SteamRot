/////////////////////////////////////////////////
/// @file
/// @brief Free functions for configuring Subscriber from FlatBuffers data
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "Subscriber.h"
#include "subscriber_generated.h"
#include <expected>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Configure a Subscriber from FlatBuffers SubscriberFbs data
///
/// @param subscriber Reference to Subscriber to populate
/// @param subscriber_fbs Pointer to SubscriberFbs FlatBuffers data
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSubscriber(Subscriber &subscriber,
                    const SubscriberFbs *subscriber_fbs);

/////////////////////////////////////////////////
/// @brief Create a Subscriber from FlatBuffers SubscriberFbs data
///
/// @param subscriber_fbs Pointer to SubscriberFbs FlatBuffers data
/// @return Subscriber on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<Subscriber, FailInfo>
CreateSubscriber(const SubscriberFbs *subscriber_fbs);

} // namespace steamrot::data::configure
