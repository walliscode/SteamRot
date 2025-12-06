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
#include "ISceneDataProvider.h"
#include "SceneCore.h"
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

/////////////////////////////////////////////////
/// @brief Configure SceneCore from native SceneCoreData struct.
///
/// Configures render texture dimensions from the scene data.
/// The render texture is created directly during configuration.
///
/// @param scene_core SceneCore instance to configure
/// @param core_data Reference to native SceneCoreData configuration
/// @return Expected containing monostate or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneCore(SceneCore &scene_core, const SceneCoreData &core_data);

} // namespace core
} // namespace steamrot
