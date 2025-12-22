/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneDataProvider.h"
#include "scene_types_generated.h"

namespace steamrot {
class FlatbuffersSceneDataProvider : public steamrot::ISceneDataProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Loads SceneData from FlatBuffers default data files.
  ///
  /// @param scene_type The type of scene to load
  /// @return Unique pointer to SceneData object.
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const override;

  /////////////////////////////////////////////////
  /// @brief Provides SceneData from SaveData.
  ///
  /// Extracts the scene_data from SaveData by moving it out.
  /// This transfers ownership of the SceneData to the caller.
  ///
  /// @param save_data SaveData containing scene configuration
  /// @return Unique pointer to SceneData object extracted from save data
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideSceneDataFromSave(SaveData &save_data) const override;
};
} // namespace steamrot
