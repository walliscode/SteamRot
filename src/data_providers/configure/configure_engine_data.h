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
#include "EngineResourcesConfig.h"
#include "FailInfo.h"
#include "engine_resources_config_generated.h"
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

} // namespace steamrot::data::configure
