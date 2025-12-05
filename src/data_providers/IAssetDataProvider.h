/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading asset configuration data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "scene_types_generated.h"
#include <expected>
#include <string>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct FontData
/// @brief Font asset information.
/////////////////////////////////////////////////
struct FontData {
  std::string name;
};

/////////////////////////////////////////////////
/// @struct AssetData
/// @brief Native C++ struct for asset configuration.
///
/// This replaces the FlatBuffers AssetCollection type in game code.
/// Provides a simple, mutable structure for asset configuration.
/////////////////////////////////////////////////
struct AssetData {
  std::vector<FontData> fonts;
  std::vector<std::string> ui_styles;
};

/////////////////////////////////////////////////
/// @class IAssetDataProvider
/// @brief Interface for loading asset configuration data.
///
/// Implementations handle the actual data source (files, network, etc.)
/// and format (FlatBuffers, JSON, XML, Lua, etc.).
///
/// Usage:
/// ```cpp
/// IAssetDataProvider& provider = GetAssetDataProvider();
/// auto result = provider.LoadAssetData();
/// if (result.has_value()) {
///   const AssetData& data = result.value();
///   // Use native C++ struct
/// }
/// ```
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
