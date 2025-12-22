/////////////////////////////////////////////////
/// @file
/// @brief Implementation of ISceneDataProvider interface.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FailInfo.h"
#include "SaveData.h"
#include "SceneData.h"
#include "scene_types_generated.h"
#include <expected>
#include <memory>
namespace steamrot {

class ISceneDataProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor.
  /////////////////////////////////////////////////
  virtual ~ISceneDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provides pointer to SceneData object from default data files.
  ///
  /// Loads scene data from the default scene configuration files
  /// (e.g., scene_data.json). Used for starting a new game or loading
  /// a scene in its default state.
  ///
  /// @param scene_type The type of scene to load
  /// @return SceneData or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;

  /////////////////////////////////////////////////
  /// @brief Provides pointer to SceneData object from save data.
  ///
  /// Extracts and returns the SceneData from SaveData. The SceneData
  /// contained in SaveData may be polymorphic (e.g., FbsSceneData).
  ///
  /// This method transfers ownership of the scene_data from SaveData
  /// to the caller via std::move.
  ///
  /// @param save_data SaveData containing the scene configuration
  /// @return SceneData extracted from save data or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideSceneDataFromSave(SaveData &save_data) const = 0;
};
} // namespace steamrot
