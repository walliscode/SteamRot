////////////////////////////////////////////////////////////
/// @file
/// @brief Type alias for the input-action registry.
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "EventPayload.h"
#include "UserInputBitset.h"
#include <unordered_map>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Maps unique input patterns to the InputAction they produce.
///
/// Each UserInputBitset key is unique — if the same pattern is registered
/// twice the later registration overwrites the earlier one. InputAction
/// values can be repeated across different keys.
////////////////////////////////////////////////////////////
using InputActionRegistry =
    std::unordered_map<UserInputBitset, InputPayload::InputAction>;

} // namespace steamrot
