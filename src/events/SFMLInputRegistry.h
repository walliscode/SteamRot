/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SFMLInputRegistry class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacket.h"
#include "SFMLInputBinding.h"
#include <SFML/Window/Event.hpp>
#include <set>
#include <unordered_set>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class SFMLInputRegistry
/// @brief Converts SFML window events into InputPayload EventPackets.
///
/// Holds a configurable registry of SFMLInputBindings. On each SFML event, the
/// registry updates its internal held-key/button state and checks all bindings
/// to determine which actions should fire.
///
/// Key features:
/// - AND logic: an action fires only when all its required_inputs are held
/// - Pressed/Released: each binding specifies whether it fires on press or
///   release of the combination
/// - Runtime configuration: bindings can be replaced at any time via Configure()
/////////////////////////////////////////////////
class SFMLInputRegistry {

private:
  /////////////////////////////////////////////////
  /// @brief The configured input bindings
  /////////////////////////////////////////////////
  std::vector<SFMLInputBinding> m_bindings;

  /////////////////////////////////////////////////
  /// @brief Currently held keyboard keys
  /////////////////////////////////////////////////
  std::set<sf::Keyboard::Key> m_held_keys;

  /////////////////////////////////////////////////
  /// @brief Currently held mouse buttons
  /////////////////////////////////////////////////
  std::set<sf::Mouse::Button> m_held_buttons;

  /////////////////////////////////////////////////
  /// @brief Indices of bindings that are currently fully satisfied
  /////////////////////////////////////////////////
  std::unordered_set<size_t> m_active_binding_indices;

  /////////////////////////////////////////////////
  /// @brief Check whether all required inputs of a binding are currently held.
  ///
  /// @param binding The binding to evaluate
  /// @return true if every required input is in the held set
  /////////////////////////////////////////////////
  bool IsBindingSatisfied(const SFMLInputBinding &binding) const;

  /////////////////////////////////////////////////
  /// @brief Compare current satisfaction state of all bindings against the
  /// previous state and generate EventPackets for any transitions.
  ///
  /// @return Vector of EventPackets for bindings that just transitioned
  /////////////////////////////////////////////////
  std::vector<EventPacket> CheckBindings();

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  SFMLInputRegistry() = default;

  /////////////////////////////////////////////////
  /// @brief Replace the current bindings with a new set.
  ///
  /// Resets all tracked input state so that no spurious events are generated
  /// from the previous configuration.
  ///
  /// @param bindings New bindings to use for input conversion
  /////////////////////////////////////////////////
  void Configure(const std::vector<SFMLInputBinding> &bindings);

  /////////////////////////////////////////////////
  /// @brief Process a single SFML event and return any triggered EventPackets.
  ///
  /// Only keyboard and mouse button events produce output; all other SFML event
  /// types are silently ignored.
  ///
  /// @param event The SFML event to process
  /// @return Vector of EventPackets triggered by this event (may be empty)
  /////////////////////////////////////////////////
  std::vector<EventPacket> ProcessSFMLEvent(const sf::Event &event);

  /////////////////////////////////////////////////
  /// @brief Return the currently configured bindings.
  ///
  /// @return Const reference to the bindings vector
  /////////////////////////////////////////////////
  const std::vector<SFMLInputBinding> &GetBindings() const;

  /////////////////////////////////////////////////
  /// @brief Return the set of currently held keyboard keys.
  ///
  /// @return Const reference to the held-keys set
  /////////////////////////////////////////////////
  const std::set<sf::Keyboard::Key> &GetHeldKeys() const;

  /////////////////////////////////////////////////
  /// @brief Return the set of currently held mouse buttons.
  ///
  /// @return Const reference to the held-buttons set
  /////////////////////////////////////////////////
  const std::set<sf::Mouse::Button> &GetHeldButtons() const;
};

} // namespace steamrot
