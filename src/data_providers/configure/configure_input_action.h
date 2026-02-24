////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of input-action configure free functions.
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "EventPayload.h"
#include "FailInfo.h"
#include "InputActionRegistry.h"
#include "UserInputBitset.h"
#include "input_action_config_generated.h"
#include <expected>

namespace steamrot::data::configure {

////////////////////////////////////////////////////////////
/// @brief Convert an InputActionFbs enum value to a native InputAction.
///
/// Shared helper used by both ConfigureInputPayload and
/// ConfigureInputActionMapping so the mapping only exists in one place.
///
/// @param action     Output: native InputAction value.
/// @param action_fbs InputActionFbs enum value to convert.
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputAction(InputPayload::InputAction &action,
                     InputActionFbs action_fbs);

////////////////////////////////////////////////////////////
/// @brief Configure a single (pattern, action) entry from FlatBuffers data.
///
/// @param bitset        UserInputBitset to populate with the input pattern.
/// @param action        InputAction output value.
/// @param mapping_data  FlatBuffers source data; must not be null.
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputActionMapping(UserInputBitset &bitset,
                            InputPayload::InputAction &action,
                            const InputActionMappingFbs *mapping_data);

////////////////////////////////////////////////////////////
/// @brief Build an InputActionRegistry from FlatBuffers config data.
///
/// @param registry    Registry to populate.
/// @param config_data FlatBuffers source data; must not be null.
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureInputActionRegistry(InputActionRegistry &registry,
                             const InputActionConfigFbs *config_data);

} // namespace steamrot::data::configure
