/////////////////////////////////////////////////
/// @file
/// @brief Decleration of the EventPacket struct
/////////////////////////////////////////////////

#pragma once

#include "EventTypes.h"
#include "event_helpers.h"
#include "uuid.h"
#include <optional>
#include <variant>
namespace steamrot {

// int is a placeholder for now. remove when you have a real event data type
using EventData = std::variant<UserInputBitset>;

struct EventPacket {

  /////////////////////////////////////////////////
  /// @brief Constructor for the EventPacket
  ///
  /// @param lifetime The lifetime of the event in ticks.
  /////////////////////////////////////////////////
  EventPacket(uint8_t lifetime) : event_lifetime(lifetime) {}

  /////////////////////////////////////////////////
  /// @brief Enum based name for the event type
  /////////////////////////////////////////////////
  EventType m_event_type{EventType::NoEvent};

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

  std::optional<EventData> m_event_data;

  const uint8_t GetLifetime() const { return event_lifetime; }

  void DecrementLifetime() {
    if (event_lifetime > 0) {
      --event_lifetime;
    }
  }

private:
  /////////////////////////////////////////////////
  /// @brief Event of the lifetime, will be ticked down 1 each cycle
  /////////////////////////////////////////////////
  uint8_t event_lifetime{1};
};
} // namespace steamrot
