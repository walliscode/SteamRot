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
#include "InputActionRegistry.h"
#include "UserInputBitset.h"
#include <SFML/Window/Event.hpp>
#include <vector>

namespace steamrot {

class SFMLEventConverter {

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  SFMLEventConverter() = default;

  /////////////////////////////////////////////////
  /// @brief Set the input-action registry by move.
  ///
  /// The registry is built externally (by IInputActionConfigProvider) and
  /// moved into this converter. Must be called before ConvertSFMLEvents()
  /// produces any InputPayload events.
  ///
  /// @param registry Populated InputActionRegistry to take ownership of.
  /////////////////////////////////////////////////
  void SetInputActionRegistry(InputActionRegistry &&registry);

  /////////////////////////////////////////////////
  /// @brief Convert a tick's worth of SFML events into EventPackets.
  ///
  /// The conversion pipeline, executed in order:
  ///   1. Collect keyboard / mouse SFML events into the waiting-room
  ///      bitset.
  ///   2. Resolve an InputAction from the registry.
  ///   3. Create an EventPacket for each resolved action.
  ///   4. Convert window-close event to SystemPayload::QUIT.
  ///   5. Convert window-resize event to SystemPayload::RESIZE.
  ///   6. Reset the waiting-room bitset for the next tick.
  ///
  /// @param sfml_events SFML events gathered this tick.
  /// @return EventPackets to be added to the EventHandler waiting room.
  /////////////////////////////////////////////////
  std::vector<EventPacket>
  ConvertSFMLEvents(const std::vector<sf::Event> &sfml_events);

private:
  /////////////////////////////////////////////////
  /// @brief Input-action registry loaded at startup.
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
