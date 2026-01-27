/////////////////////////////////////////////////
/// @file
/// @brief Implementation of EventTypeFbs to EventType conversion
/////////////////////////////////////////////////

#include "event_type_conversion.h"
#include "EventType.h"
#include "FailInfo.h"
#include "events_generated.h"
#include <expected>

namespace steamrot::event {

/////////////////////////////////////////////////
std::expected<EventType, FailInfo>
ConvertEventTypeFbsToEventType(EventTypeFbs event_type_fbs) {

  switch (event_type_fbs) {
  case EventTypeFbs_EVENT_NONE:
    return EventType::NONE;
  case EventTypeFbs_EVENT_TEST:
    return EventType::TEST;
  case EventTypeFbs_EVENT_USER_INPUT:
    return EventType::USER_INPUT;
  case EventTypeFbs_EVENT_TOGGLE_UI:
    return EventType::TOGGLE_UI;
  case EventTypeFbs_EVENT_CHANGE_SCENE:
    return EventType::CHANGE_SCENE;
  case EventTypeFbs_EVENT_QUIT_GAME:
    return EventType::QUIT_GAME;
  case EventTypeFbs_EVENT_TOGGLE_DROPDOWN:
    return EventType::TOGGLE_DROPDOWN;
  default:
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "ConvertEventTypeFbsToEventType: Unknown EventTypeFbs value"});
  }
}

} // namespace steamrot::event
