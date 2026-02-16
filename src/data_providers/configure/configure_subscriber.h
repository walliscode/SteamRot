/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for configuring subscriber based data
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
#include <variant>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Configures a Subscriber based on the provided flatbuffer data.
///
/// @param subscriber Subscriber to be configured
/// @param fb_subscriber SubscriberFbs flatbuffer data containing configuration
/// information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSubscriber(Subscriber &subscriber,
                     const SubscriberFbs *fb_subscriber);
} // namespace steamrot::data::configure
