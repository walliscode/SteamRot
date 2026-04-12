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
  USER_INPUT,
  UI,
  SCENE,
  LOGIC,
  SYSTEM,
  GHOST,
  CAMERA

};

} // namespace steamrot
