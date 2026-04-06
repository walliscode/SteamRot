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
#include "EventPacket.h"
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
UserInputBitset
CollectInputEvents(const std::vector<sf::Event> &sfml_events);

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
/// @brief Return true if a window-close event is present in @p sfml_events.
///
/// @param sfml_events SFML events gathered this tick.
/// @return True when an sf::Event::Closed event is found.
/////////////////////////////////////////////////
bool HasWindowCloseEvent(const std::vector<sf::Event> &sfml_events);

/////////////////////////////////////////////////
/// @brief Extract the new window size from a resize event, if present.
///
/// Searches @p sfml_events for an sf::Event::Resized event and returns the
/// new window dimensions.  Returns std::nullopt when no resize event is
/// found in the list.
///
/// @param sfml_events SFML events gathered this tick.
/// @return New window size, or std::nullopt if no resize event occurred.
/////////////////////////////////////////////////
std::optional<sf::Vector2u>
ExtractWindowResizeSize(const std::vector<sf::Event> &sfml_events);

} // namespace steamrot::events::convert
