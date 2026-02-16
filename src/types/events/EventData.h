/////////////////////////////////////////////////
/// @file
/// @brief Declaration of EventData variant and related types
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneType.h"
#include "UserInputBitset.h"
#include "uuid.h"
#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Represents a scene change event with optional UUID and scene type
///
/// First element: Optional UUID of the target scene
/// Second element: SceneType of the target scene
/////////////////////////////////////////////////
using SceneChangePacket = std::pair<std::optional<uuids::uuid>, SceneType>;

/////////////////////////////////////////////////
/// @brief Represents a user interface name identifier
/////////////////////////////////////////////////
using UserInterfaceName = std::string;

/////////////////////////////////////////////////
/// @brief Represents a toggle name identifier for logic toggles
/////////////////////////////////////////////////
using ToggleName = std::string;

/////////////////////////////////////////////////
/// @brief Variant type for event-specific data
///
/// EventData can hold different types of event data or std::monostate for
/// events without specific data. This is used by Subscribers and the event
/// system to pass type-safe event information.
/////////////////////////////////////////////////
using EventData = std::variant<std::monostate, UserInputBitset,
                                SceneChangePacket, UserInterfaceName,
                                ToggleName>;

} // namespace steamrot
