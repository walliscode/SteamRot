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
#include "FailInfo.h"
#include "UserInputBitset.h"
#include "event_payload_generated.h"
#include "input_action_config_generated.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <expected>
#include <optional>
#include <unordered_map>
#include <vector>

namespace steamrot::events::convert {

/////////////////////////////////////////////////
/// @brief Maps unique input patterns to the InputAction they produce.
///
/// Each UserInputBitset key is unique — if the same pattern is registered
/// twice the later registration overwrites the earlier one.  InputAction
/// values can be repeated across different keys.
/////////////////////////////////////////////////
using InputActionRegistry =
    std::unordered_map<UserInputBitset, InputPayload::InputAction>;

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
/// @param registry      Ordered list of (pattern → action) mappings.
/// @return The matched InputAction, or std::nullopt when nothing matches.
/////////////////////////////////////////////////
std::optional<InputPayload::InputAction>
ResolveInputAction(const UserInputBitset &accumulated,
                   const InputActionRegistry &registry);

/////////////////////////////////////////////////
/// @brief Convert an InputActionFbs enum value to a native InputAction.
///
/// Shared helper used by both ConfigureInputPayload (in configure_event) and
/// ConfigureInputActionMapping so the mapping only exists in one place.
///
/// @param action      Output: native InputAction value.
/// @param action_fbs  InputActionFbs enum value to convert.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputAction(InputPayload::InputAction &action,
                     InputActionFbs action_fbs);

/////////////////////////////////////////////////
/// @brief Configure a single (pattern, action) entry from FlatBuffers data.
///
/// Populates @p bitset with the key/button bits described by @p mapping_data,
/// and sets @p action to the corresponding InputAction.
///
/// @param bitset        UserInputBitset to populate with the input pattern.
/// @param action        InputAction output value.
/// @param mapping_data  FlatBuffers source data; must not be null.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputActionMapping(UserInputBitset &bitset,
                            InputPayload::InputAction &action,
                            const InputActionMappingFbs *mapping_data);

/////////////////////////////////////////////////
/// @brief Build an InputActionRegistry from FlatBuffers config data.
///
/// Iterates all mappings in @p config_data and appends the converted
/// entries to @p registry in order.
///
/// @param registry    Registry to populate.
/// @param config_data FlatBuffers source data; must not be null.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputActionRegistry(InputActionRegistry &registry,
                             const InputActionConfigFbs *config_data);

} // namespace steamrot::events::convert
