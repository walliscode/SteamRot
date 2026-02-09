/////////////////////////////////////////////////
/// @file
/// @brief Decleration of the EventPacket struct
/////////////////////////////////////////////////

#pragma once

#include "EventType.h"
#include "SceneType.h"
#include "UserInputBitset.h"
#include "uuid.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <optional>

#include <variant>
namespace steamrot {

using SceneChangePacket = std::pair<std::optional<uuids::uuid>, SceneType>;
using UserInterfaceName = std::string;
/////////////////////////////////////////////////
//// @brief names and toggles to be used for flow control
/////////////////////////////////////////////////
using ToggleName = std::string;

// Your variant type
using EventData =
    std::variant<std::monostate, UserInputBitset, SceneChangePacket,
                 UserInterfaceName, ToggleName>;

struct EventPacket {

  /////////////////////////////////////////////////
  /// @brief Constructor for the EventPacket
  ///
  /// @param lifetime The lifetime of the event in ticks.
  /////////////////////////////////////////////////
  EventPacket(uint8_t lifetime) : event_lifetime(lifetime) {}

  EventPacket(EventType event_type, EventData event_data, uint8_t lifetime = 1)
      : event_type(event_type), event_data(event_data),
        event_lifetime(lifetime) {}
  /////////////////////////////////////////////////
  /// @brief Enum based name for the event type
  /////////////////////////////////////////////////
  EventType event_type{EventType::NONE};

  /////////////////////////////////////////////////
  /// @brief Specific data for the event
  /////////////////////////////////////////////////
  EventData event_data{std::monostate{}};

  /////////////////////////////////////////////////
  /// @brief Unique identifier for the event
  /////////////////////////////////////////////////
  uuids::uuid event_id;

  /////////////////////////////////////////////////
  /// @brief Unique identifier for the source of the event
  ///
  /// This is to match the event origin as we have a global event bus
  /////////////////////////////////////////////////
  uuids::uuid source_id;

  /////////////////////////////////////////////////
  /// @brief Event of the lifetime, will be ticked down 1 each cycle
  /////////////////////////////////////////////////
  uint8_t event_lifetime{1};
};
} // namespace steamrot
