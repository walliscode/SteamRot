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
  /// @brief Loads SceneData from FlatBuffers.
  ///
  /// @return Unique pointer to SceneData object.
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const override;
};
} // namespace steamrot
