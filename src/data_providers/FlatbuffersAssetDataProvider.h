/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of IAssetDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "IAssetDataProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersAssetDataProvider
/// @brief FlatBuffers implementation of IAssetDataProvider.
///
/// Loads asset data from FlatBuffers binary files and converts
/// to native C++ structs for use by game code.
/////////////////////////////////////////////////
class FlatbuffersAssetDataProvider : public IAssetDataProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Instance of FlatbuffersDataLoader to handle data loading.
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_loader;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor.
  /////////////////////////////////////////////////
  FlatbuffersAssetDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Returns AssetData struct for parsing
  /////////////////////////////////////////////////
  std::expected<AssetData, FailInfo> LoadAssetData() const override;

  /////////////////////////////////////////////////
  /// @brief Provides Assetdata for a specific  default scene type.
  ///
  /// @param scene_type SceneType enum value specifying which scene to load.
  /////////////////////////////////////////////////
  std::expected<AssetData, FailInfo>
  LoadSceneAssetData(SceneType scene_type) const override;
};

} // namespace steamrot
