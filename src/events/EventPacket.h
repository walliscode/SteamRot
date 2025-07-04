/////////////////////////////////////////////////
/// @file
/// @brief Decleration of the EventPacket struct
/////////////////////////////////////////////////

#pragma once

#include "uuid.h"
#include <optional>
#include <variant>
namespace steamrot {

// int is a placeholder for now. remove when you have a real event data type
using EventData = std::variant<int>;

struct EventPacket {

  /////////////////////////////////////////////////
  /// @brief Unique identifier for the event
  /////////////////////////////////////////////////
  uuids::uuid event_id;

  /////////////////////////////////////////////////
  /// @brief Unique identifier for the source of the event
  ///
  /// This is to mactch the event origin as we have a gloval event bus
  /////////////////////////////////////////////////
  uuids::uuid source_id;

  /////////////////////////////////////////////////
  /// @brief Event of the lifetime, will be ticked down 1 each cycle
  /////////////////////////////////////////////////
  uint8_t event_lifetime{1};

  std::optional<EventData> m_event_data;
};
} // namespace steamrot
