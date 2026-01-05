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
  /// @brief Provides pointer to SceneData object by loading from file.
  ///
  /// @param scene_type Type of scene to load
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;

  /////////////////////////////////////////////////
  /// @brief Converts FlatBuffers SceneData to native SceneData object.
  ///
  /// This method allows passing pre-loaded FlatBuffers data directly
  /// for conversion without file I/O.
  ///
  /// @param fb_scene_data Pointer to FlatBuffers SceneDataFbs
  /// @return Unique pointer to converted SceneData, or error
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ConvertSceneData(const SceneDataFbs *fb_scene_data) const = 0;
};
} // namespace steamrot
