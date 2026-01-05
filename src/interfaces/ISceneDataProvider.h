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
#include "SceneData.h"
#include "scene_data_generated.h"
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
  /// @brief Provides pointer to SceneData object.
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;

  /////////////////////////////////////////////////
  /// @brief Virtual method to provide SceneData from FlatBuffers data.
  ///
  /// @param scene_data_fbs FlatBuffers SceneData for scene.
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideSceneDataFromData(const SceneDataFbs *scene_data_fbs) const = 0;
};
} // namespace steamrot
