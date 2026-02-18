/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for converting SFML events to InputPayload
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "EventPayload.h"
#include "InputMappingRegistry.h"
#include <SFML/Window/Event.hpp>
#include <optional>

namespace steamrot::events {

/////////////////////////////////////////////////
/// @brief Converts an SFML event to an InputPayload using the provided registry
///
/// This function examines the SFML event and, if it's a keyboard or mouse
/// input event, looks up the corresponding InputAction in the registry.
///
/// @param event The SFML event to convert
/// @param registry The InputMappingRegistry to use for the conversion
/// @return An InputPayload if the event maps to an action, std::nullopt otherwise
/////////////////////////////////////////////////
std::optional<InputPayload>
ConvertSFMLEventToInputPayload(const sf::Event &event,
                               const InputMappingRegistry &registry);

/////////////////////////////////////////////////
/// @brief Converts an SFML event to an InputPayload using default mappings
///
/// This is a convenience function that creates a temporary registry with
/// default mappings. For performance-critical code, prefer creating a
/// registry once and reusing it with the two-parameter version.
///
/// @param event The SFML event to convert
/// @return An InputPayload if the event maps to an action, std::nullopt otherwise
/////////////////////////////////////////////////
std::optional<InputPayload>
ConvertSFMLEventToInputPayload(const sf::Event &event);

} // namespace steamrot::events
