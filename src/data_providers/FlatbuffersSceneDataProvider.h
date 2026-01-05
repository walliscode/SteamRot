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
#include "SceneResourcesConfig.h"
#include "scene_resources_config_generated.h"
#include "scene_types_generated.h"
#include <memory>

namespace steamrot {
class FlatbuffersSceneDataProvider : public steamrot::ISceneDataProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Configures SceneResourcesConfig from FlatBuffers data.
  ///
  /// @param config SceneResourcesConfig to configure.
  /// @param fb_config FlatBuffers SceneResourcesConfig data.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResourcesConfig(SceneResourcesConfig &config,
                                const SceneResourcesConfigFbs *fb_config) const;

  /////////////////////////////////////////////////
  /// @brief Loads SceneData from FlatBuffers file.
  ///
  /// @param scene_type Type of scene to load
  /// @return Unique pointer to SceneData object.
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const override;

  /////////////////////////////////////////////////
  /// @brief Converts FlatBuffers SceneData to native SceneData object.
  ///
  /// This method allows passing pre-loaded FlatBuffers data directly
  /// for conversion without file I/O.
  ///
  /// @param fb_scene_data Pointer to FlatBuffers SceneDataFbs
  /// @return Unique pointer to converted SceneData, or error
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<SceneData>, FailInfo>
  ConvertSceneData(const SceneDataFbs *fb_scene_data) const override;
};
} // namespace steamrot
