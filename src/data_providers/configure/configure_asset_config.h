/////////////////////////////////////////////////
/// @file
/// @brief Declaration of functions to configure asset data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetConfig.h"
#include "FailInfo.h"
#include "asset_config_generated.h"
#include <expected>
#include <variant>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Configure an AssetConfig struct from FlatBuffers data.
///
/// @param asset_config AssetConfig struct to populate.
/// @param asset_config_fb_data AssetConfigFbs FlatBuffers data source.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureAssetConfig(AssetConfig &asset_config,
                     const AssetConfigFbs *asset_config_fb_data);

} // namespace steamrot::data::configure
