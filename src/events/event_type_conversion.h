/////////////////////////////////////////////////
/// @file
/// @brief Conversion utilities between EventTypeFbs and EventType
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventType.h"
#include "FailInfo.h"
#include "events_generated.h"
#include <expected>

namespace steamrot::event {

/////////////////////////////////////////////////
/// @brief Convert EventTypeFbs to native EventType
///
/// Converts the FlatBuffers EventTypeFbs enum to the native C++
/// EventType enum. This function provides the bridge between the
/// serialization layer and the runtime code.
///
/// @param event_type_fbs FlatBuffers EventTypeFbs to convert
/// @return Native EventType on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<EventType, FailInfo>
ConvertEventTypeFbsToEventType(EventTypeFbs event_type_fbs);

} // namespace steamrot::event
