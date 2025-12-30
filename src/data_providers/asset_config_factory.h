/////////////////////////////////////////////////
/// @file
/// @brief Free functions for creating/pouplating AssetConfig structs.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "AssetConfig.h"
#include "FailInfo.h"
#include "asset_config_generated.h"
#include <expected>
#include <variant>
namespace steamrot {

/////////////////////////////////////////////////
/// @brief Configure an AssetConfig struct from FlatBuffers data.
///
/// @param asset_config AssetConfig struct to populate.
/// @param asset_config_fb_data AssetConfigFbs FlatBuffers data source.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureAssetConfig(AssetConfig &asset_config,
                     const AssetConfigFbs *asset_config_fb_data);
} // namespace steamrot
