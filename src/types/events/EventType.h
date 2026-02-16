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

  // User Input (100-199)
  USER_INPUT_KEYBOARD = 100,
  USER_INPUT_MOUSE = 101,
  USER_INPUT_GAMEPAD = 102,

  // UI (200-299)
  UI_TOGGLE = 200,

  // Scene (300-399)
  SCENE_CHANGE = 300,

  // Logic (400-499)
  LOGIC_TOGGLE = 400,

  // System (500-599)
  SYSTEM_QUIT = 500,

};

} // namespace steamrot
