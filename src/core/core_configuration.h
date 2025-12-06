/////////////////////////////////////////////////
/// @file
/// @brief Free functions for configuring core objects from native data structs.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineResources.h"
#include "FailInfo.h"
#include "IEngineDataProvider.h"
#include <expected>

namespace steamrot {
namespace core {

/////////////////////////////////////////////////
/// @brief Configure EngineResources from config data.
///
/// Configures window settings (size, title, framerate) from the
/// configuration data. The window is created directly during configuration.
///
/// @param engine_resources EngineResources instance to configure
/// @param config_data Reference to EngineResourcesConfigData
/// @return Expected containing monostate or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineResources(EngineResources &engine_resources,
                         const EngineResourcesConfigData &config_data);

} // namespace core
} // namespace steamrot
