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

namespace steamrot {

class ISceneLoadDataProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor.
  /////////////////////////////////////////////////
  virtual ~ISceneLoadDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provides pointer to SceneData object.
  /////////////////////////////////////////////////
  virtual std::expected<SceneData, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;

  /////////////////////////////////////////////////
  /// @brief Provides a SceneData object from flatbuffers data
  ///
  /// Designed to be overloaded and then overriden per concrete Implementation
  /// @param fb_data flatbuffers data
  /////////////////////////////////////////////////
  virtual std::expected<SceneData, FailInfo>
  ProvideSceneDataFromData(const SceneDataFbs *fb_data) const = 0;
};
} // namespace steamrot
