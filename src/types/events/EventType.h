/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the native EventType enum
/////////////////////////////////////////////////

#pragma once

#include <cstdint>

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
  TEST = 1ULL << 0,
  USER_INPUT = 1ULL << 1,
  TOGGLE_UI = 1ULL << 2,
  CHANGE_SCENE = 1ULL << 3,
  QUIT_GAME = 1ULL << 4,
  TOGGLE_DROPDOWN = 1ULL << 5
};

} // namespace steamrot
