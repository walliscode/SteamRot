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
#include "FailInfo.h"
#include "Subscriber.h"
#include "subscriber_generated.h"
#include <expected>

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

} // namespace subscriber_factory
} // namespace steamrot
