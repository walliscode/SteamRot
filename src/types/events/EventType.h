/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the native EventType enum
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @enum EventType
/// @brief Native C++ enum representing event types in the game engine
///
/// This is the native representation used throughout the codebase.
/// FlatBuffers data is converted to this type via conversion functions.
/////////////////////////////////////////////////
enum class EventType : uint64_t {
  EVENT_NONE = 1ULL,
  EVENT_TEST = 2ULL,
  EVENT_USER_INPUT = 4ULL,
  EVENT_TOGGLE_UI = 8ULL,
  EVENT_CHANGE_SCENE = 16ULL,
  EVENT_QUIT_GAME = 32ULL,
  EVENT_TOGGLE_DROPDOWN = 64ULL,
};

/////////////////////////////////////////////////
/// @brief Convert EventType to string name
///
/// @param event_type The EventType to convert
/// @return String representation of the event type
/////////////////////////////////////////////////
inline const char *EnumNameEventType(EventType event_type) {
  switch (event_type) {
  case EventType::EVENT_NONE:
    return "EVENT_NONE";
  case EventType::EVENT_TEST:
    return "EVENT_TEST";
  case EventType::EVENT_USER_INPUT:
    return "EVENT_USER_INPUT";
  case EventType::EVENT_TOGGLE_UI:
    return "EVENT_TOGGLE_UI";
  case EventType::EVENT_CHANGE_SCENE:
    return "EVENT_CHANGE_SCENE";
  case EventType::EVENT_QUIT_GAME:
    return "EVENT_QUIT_GAME";
  case EventType::EVENT_TOGGLE_DROPDOWN:
    return "EVENT_TOGGLE_DROPDOWN";
  default:
    return "UNKNOWN";
  }
}

} // namespace steamrot

/////////////////////////////////////////////////
/// @brief Hash specialization for EventType to enable use in unordered_map
/////////////////////////////////////////////////
namespace std {
template <> struct hash<steamrot::EventType> {
  size_t operator()(const steamrot::EventType &event_type) const {
    return hash<uint64_t>()(static_cast<uint64_t>(event_type));
  }
};
} // namespace std
