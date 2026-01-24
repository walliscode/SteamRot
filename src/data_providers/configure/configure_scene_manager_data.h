/////////////////////////////////////////////////
/// @file
/// @brief Declaration of functions to configure scene manager data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneManagerState.h"
#include "scene_manager_state_generated.h"
#include <expected>
#include <variant>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Configure SceneManagerState from FlatBuffers data.
///
/// @param state SceneManagerState to configure
/// @param state_data SceneManagerStateFbs FlatBuffers data to use for
/// configuration
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneManagerState(SceneManagerState &state,
                           const SceneManagerStateFbs *state_data);
} // namespace steamrot::data::configure
