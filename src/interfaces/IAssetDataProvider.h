/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading asset configuration data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetData.h"
#include "FailInfo.h"
#include "scene_types_generated.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IAssetDataProvider
/// @brief Interface for loading asset configuration data.
///
/// Implementations handle the actual data source (files, network, etc.)
/// and format (FlatBuffers, JSON, XML, Lua, etc.).
/////////////////////////////////////////////////
class IAssetDataProvider {
public:
  virtual ~IAssetDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load default asset data.
  ///
  /// @return Asset data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<AssetData, FailInfo> LoadAssetData() const = 0;

  /////////////////////////////////////////////////
  /// @brief Load scene-specific asset data.
  ///
  /// @param scene_type The type of scene to load assets for
  /// @return Asset data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<AssetData, FailInfo>
  LoadSceneAssetData(SceneType scene_type) const = 0;
};

} // namespace steamrot
