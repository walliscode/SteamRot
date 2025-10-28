////////////////////////////////////////////////////////////
/// @file
/// @brief Free functions for configuring resources from FlatBuffers data.
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "FailInfo.h"
#include "GameResources.h"
#include "SceneResources.h"
#include "resource_data_generated.h"
#include "scene_change_packet_generated.h"
#include <expected>
#include <string>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Configure GameResources from FlatBuffers data.
///
/// Configures window settings (size, title, framerate) from the
/// configuration data. The window is created directly during configuration.
///
/// @param resources GameResources instance to configure
/// @param game_data Pointer to GameResourcesData configuration
/// @return Expected containing monostate or FailInfo on error
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureGameResources(GameResources &resources,
                       const GameResourcesData *game_data);

////////////////////////////////////////////////////////////
/// @brief Configure SceneResources from FlatBuffers data.
///
/// Configures render texture dimensions from the scene data.
/// If scene_data is null, uses default dimensions (800x600).
/// The render texture is created directly during configuration.
///
/// @param resources SceneResources instance to configure
/// @param scene_data Scene resource data (can be null for defaults)
/// @return Expected containing monostate or FailInfo on error
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneResources(SceneResources &resources,
                        const SceneResourcesData *scene_data);

} // namespace steamrot
