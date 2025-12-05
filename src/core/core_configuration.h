/////////////////////////////////////////////////
/// @file
/// @brief Free functions for configuring core objects from native data structs.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "GameCore.h"
#include "IEngineDataProvider.h"
#include "ISceneDataProvider.h"
#include "SceneCore.h"
#include <expected>

namespace steamrot {
namespace core {

/////////////////////////////////////////////////
/// @brief Configure GameCore from native EngineCoreData struct.
///
/// Configures window settings (size, title, framerate) from the
/// configuration data. The window is created directly during configuration.
///
/// @param game_core GameCore instance to configure
/// @param core_data Reference to native EngineCoreData configuration
/// @return Expected containing monostate or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureGameCore(GameCore &game_core, const EngineCoreData &core_data);

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
