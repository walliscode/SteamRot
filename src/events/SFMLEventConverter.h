/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SFMLEventConverter class.
///
/// SFMLEventConverter coordinates the conversion of raw SFML events
/// into engine EventPackets.  It gathers the conversion free-functions
/// (from sfml_event_convert.h) in an easily visible order and holds
/// the registries / conversion charts needed for the mappings.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacket.h"
#include "EventPayload.h"
#include "FailInfo.h"
#include "UserInputBitset.h"
#include "sfml_event_convert.h"
#include <SFML/Window/Event.hpp>
#include <expected>
#include <vector>

namespace steamrot {

class SFMLEventConverter {

public:
  /////////////////////////////////////////////////
  /// @brief Type alias for the input-action registry used by this class.
  ///
  /// Each entry maps a UserInputBitset pattern to the InputAction it
  /// should produce.  Patterns are evaluated in order; the first match
  /// wins.
  /////////////////////////////////////////////////
  using InputActionRegistry = events::convert::InputActionRegistry;

  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  SFMLEventConverter() = default;

  /////////////////////////////////////////////////
  /// @brief Load the input-action registry from FlatBuffers config data.
  ///
  /// Coordinates calling the configure free-functions from sfml_event_convert
  /// to populate the internal registry from the provided FlatBuffers data.
  /// Must be called before ConvertSFMLEvents() produces any InputPayload
  /// events.
  ///
  /// @param config_data Pointer to InputActionConfigFbs flatbuffers data.
  /// @return std::monostate on success, FailInfo on error.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  Configure(const InputActionConfigFbs *config_data);

  /////////////////////////////////////////////////
  /// @brief Convert a tick's worth of SFML events into EventPackets.
  ///
  /// The conversion pipeline, executed in order:
  ///   1. Collect keyboard / mouse SFML events into the waiting-room
  ///      bitset.
  ///   2. Resolve an InputAction from the registry.
  ///   3. Create an EventPacket for each resolved action.
  ///   4. Reset the waiting-room bitset for the next tick.
  ///
  /// @param sfml_events SFML events gathered this tick.
  /// @return EventPackets to be added to the EventHandler waiting room.
  /////////////////////////////////////////////////
  std::vector<EventPacket>
  ConvertSFMLEvents(const std::vector<sf::Event> &sfml_events);

private:
  /////////////////////////////////////////////////
  /// @brief Ordered input-action registry loaded at startup.
  /////////////////////////////////////////////////
  InputActionRegistry m_input_action_registry;

  /////////////////////////////////////////////////
  /// @brief Accumulates keyboard/mouse bits for the current tick.
  ///
  /// Reset at the end of every ConvertSFMLEvents() call.
  /////////////////////////////////////////////////
  UserInputBitset m_waiting_room_bitset;
};

} // namespace steamrot
