/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for converting SFML events.
///
/// These functions contain the specific conversion logic (as opposed
/// to ordering logic) for translating SFML events into engine types.
/// Each function is independently unit-testable.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayload.h"
#include "InputActionRegistry.h"
#include "UserInputBitset.h"
#include <SFML/Window/Event.hpp>
#include <optional>
#include <vector>

namespace steamrot::events::convert {

/////////////////////////////////////////////////
/// @brief Collect keyboard and mouse SFML events into a UserInputBitset.
///
/// Iterates over all SFML events from a single tick and encodes each
/// key/button press and release into the returned bitset.
///
/// @param sfml_events Vector of SFML events gathered this tick.
/// @return UserInputBitset encoding all input state from the events.
/////////////////////////////////////////////////
UserInputBitset CollectInputEvents(const std::vector<sf::Event> &sfml_events);

/////////////////////////////////////////////////
/// @brief Resolve an InputAction from a bitset using the registry.
///
/// Each registry entry is checked to see whether its required bits are
/// all set in @p accumulated.  The first matching entry wins.
///
/// @param accumulated   Bitset produced by CollectInputEvents for this tick.
/// @param registry      Registry of (pattern → action) mappings.
/// @return The matched InputAction, or std::nullopt when nothing matches.
/////////////////////////////////////////////////
std::optional<InputPayload::InputAction>
ResolveInputAction(const UserInputBitset &accumulated,
                   const InputActionRegistry &registry);

/////////////////////////////////////////////////
/// @brief Collect system-level SFML events (window close and Escape key).
///
/// Checks for sf::Event::Closed and sf::Event::KeyPressed with the Escape
/// key.  Either of these signals that the user wants to quit the application.
///
/// @param sfml_events Vector of SFML events gathered this tick.
/// @return true if a QUIT-triggering event was found, false otherwise.
/////////////////////////////////////////////////
bool CollectSystemEvents(const std::vector<sf::Event> &sfml_events);

/////////////////////////////////////////////////
/// @brief Detect a ghost-clear request from SFML events (Q key press).
///
/// Checks for sf::Event::KeyPressed with the Q key.  A Q press signals
/// that the user wants to clear the current ghost selection.
///
/// @param sfml_events Vector of SFML events gathered this tick.
/// @return true if a ghost-clear-triggering event was found, false otherwise.
/////////////////////////////////////////////////
bool CollectGhostClearEvents(const std::vector<sf::Event> &sfml_events);

/////////////////////////////////////////////////
/// @brief Accumulate mouse-wheel scroll delta from SFML events.
///
/// Sums the delta of all sf::Event::MouseWheelScrolled events in
/// @p sfml_events.  Returns std::nullopt when no scroll events occurred.
///
/// @param sfml_events Vector of SFML events gathered this tick.
/// @return Total scroll delta, or std::nullopt if no scroll events occurred.
/////////////////////////////////////////////////
std::optional<float>
CollectScrollDelta(const std::vector<sf::Event> &sfml_events);

/////////////////////////////////////////////////
/// @brief Collect WASD key-press and key-release events as CameraPayloads.
///
/// Iterates the SFML events and emits one CameraPayload per W/A/S/D key-press
/// (PAN_PRESS) or key-release (PAN_RELEASE).  Returns an empty vector when no
/// WASD events occurred this tick.
///
/// @param sfml_events Vector of SFML events gathered this tick.
/// @return Zero or more CameraPayloads representing pan press/release events.
/////////////////////////////////////////////////
std::vector<CameraPayload>
CollectPanEvents(const std::vector<sf::Event> &sfml_events);

} // namespace steamrot::events::convert
