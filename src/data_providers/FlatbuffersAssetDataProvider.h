/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of IAssetDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IAssetDataProvider.h"
#include "FlatbuffersDataLoader.h"

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
  FlatbuffersDataLoader m_loader;

public:
  FlatbuffersAssetDataProvider() = default;

  std::expected<AssetData, FailInfo> LoadAssetData() const override;

  std::expected<AssetData, FailInfo>
  LoadSceneAssetData(SceneType scene_type) const override;
};

} // namespace steamrot
