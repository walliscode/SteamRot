/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of ISceneDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneDataProvider.h"
#include "FlatbuffersDataLoader.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersSceneDataProvider
/// @brief FlatBuffers implementation of ISceneDataProvider.
///
/// Loads scene data from FlatBuffers binary files and converts
/// to native C++ structs for use by game code.
/////////////////////////////////////////////////
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
private:
  FlatbuffersDataLoader m_loader;

public:
  FlatbuffersSceneDataProvider() = default;

  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const override;

  std::expected<SceneCoreData, FailInfo>
  LoadSceneCoreData(SceneType scene_type) const override;
};

} // namespace steamrot
