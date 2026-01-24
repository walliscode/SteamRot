/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for configuring scene data
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneInfo.h"
#include "SceneResourcesConfig.h"
#include "scene_data_generated.h"
#include "scene_resources_config_generated.h"
#include <expected>
#include <variant>
namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Configures SceneInfo from FlatBuffers data.
///
/// @param info SceneInfo to configure.
/// @param fb_info FlatBuffers SceneInfo data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(SceneInfo &info, const SceneInfoFbs *fb_info);

/////////////////////////////////////////////////
/// @brief Configures SceneResourcesConfig from FlatBuffers data.
///
/// @param config SceneResourcesConfig to configure.
/// @param fb_config FlatBuffers SceneResourcesConfig data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneResourcesConfig(SceneResourcesConfig &config,
                              const SceneResourcesConfigFbs *fb_config);

} // namespace steamrot::data::configure
