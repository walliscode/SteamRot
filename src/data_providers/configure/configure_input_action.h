/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for configuring input-action data.
///
/// These functions convert FlatBuffers input-action data into the
/// native InputActionRegistry used by SFMLEventConverter.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SFMLEventConverter.h"
#include "input_action_config_generated.h"
#include <expected>

namespace steamrot::data::configure {

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
std::expected<std::monostate, FailInfo> ConfigureInputActionRegistry(
    SFMLEventConverter::InputActionRegistry &registry,
    const InputActionConfigFbs *config_data);

} // namespace steamrot::data::configure
