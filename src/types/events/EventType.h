/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the native EventType enum
/////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Native event type enumeration
///
/// This enum represents the different types of events that can be
/// triggered in the game engine. Unlike the FlatBuffers version,
/// this is the primary enum used throughout the codebase.
/////////////////////////////////////////////////
enum class EventType : uint64_t {
  NONE = 0,
  TEST,
  USER_INPUT,
  TOGGLE_UI,
  CHANGE_SCENE,
  QUIT_GAME,
  TOGGLE_DROPDOWN,
  LOGIC_TOGGLE
};

inline std::string EnumNameEventType(EventType type) {
  switch (type) {
  case EventType::NONE:
    return "NONE";
  case EventType::TEST:
    return "TEST";
  case EventType::USER_INPUT:
    return "USER_INPUT";
  case EventType::TOGGLE_UI:
    return "TOGGLE_UI";
  case EventType::CHANGE_SCENE:
    return "CHANGE_SCENE";
  case EventType::QUIT_GAME:
    return "QUIT_GAME";
  case EventType::TOGGLE_DROPDOWN:
    return "TOGGLE_DROPDOWN";
  case EventType::LOGIC_TOGGLE:
    return "LOGIC_TOGGLE";
  default:
    return "UNKNOWN";
  }
}
} // namespace steamrot
