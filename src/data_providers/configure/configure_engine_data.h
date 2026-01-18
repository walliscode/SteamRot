/////////////////////////////////////////////////
/// @file
/// @brief Declaration of functions to configure engine data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineConfig.h"
#include "EngineResourcesConfig.h"
#include "EngineState.h"
#include "FailInfo.h"
#include "engine_config_generated.h"
#include "engine_resources_config_generated.h"
#include "engine_state_generated.h"
#include <expected>
#include <variant>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Logic for populating EngineResourcesConfig from flatbuffers data
/// source.
///
/// @param engine_resources_config Reference to EngineResourcesConfig to
/// populate.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureEngineResourcesConfig(
    EngineResourcesConfig &engine_resources_config,
    const EngineResourcesConfigFbs *engine_resources_config_data);

/////////////////////////////////////////////////
/// @brief Logic for populating EngineConfig from data source.
///
/// @param engine_config Reference to EngineConfig to populate.
/// @param engine_config_data Pointer to EngineConfigFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineConfig(EngineConfig &engine_config,
                      const EngineConfigFbs *engine_config_data);

/////////////////////////////////////////////////
/// @brief Logic for populating EngineState from data source.
///
/// @param engine_state Reference to EngineState to populate.
/// @param engine_state_data Pointer to EngineStateFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineState(EngineState &engine_state,
                     const EngineStateFbs *engine_state_data);

} // namespace steamrot::data::configure
