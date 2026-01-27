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
