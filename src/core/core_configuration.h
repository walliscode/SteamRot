/////////////////////////////////////////////////
/// @file
/// @brief Free functions for configuring core objects from FlatBuffers data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "GameCore.h"
#include "SceneCore.h"
#include "core_data_generated.h"
#include <expected>

namespace steamrot {
namespace core {

/////////////////////////////////////////////////
/// @brief Configure GameCore from FlatBuffers data.
///
/// Configures window settings (size, title, framerate) from the
/// configuration data. The window is created directly during configuration.
///
/// @param game_core GameCore instance to configure
/// @param core_data Pointer to EngineCoreData configuration
/// @return Expected containing monostate or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureGameCore(GameCore &game_core, const EngineCoreData *core_data);

/////////////////////////////////////////////////
/// @brief Configure SceneCore from FlatBuffers data.
///
/// Configures render texture dimensions from the scene data.
/// If scene_data is null, uses default dimensions (800x600).
/// The render texture is created directly during configuration.
///
/// @param scene_core SceneCore instance to configure
/// @param scene_data Scene core data (can be null for defaults)
/// @return Expected containing monostate or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneCore(SceneCore &scene_core, const SceneCoreData *scene_data);

} // namespace core
} // namespace steamrot
