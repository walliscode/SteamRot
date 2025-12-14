/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersDefaultSceneConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneConfigurator.h"
#include "SceneData.h"

namespace steamrot {

class FlatbuffersSceneConfigurator : public ISceneConfigurator {

private:
public:
  FlatbuffersSceneConfigurator() = default;

  ~FlatbuffersSceneConfigurator() override = default;

  /////////////////////////////////////////////////
  /// @brief  function to configure SceneInfo struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneData *scene_data) override;

  /////////////////////////////////////////////////
  /// @brief  function to configure SceneResources struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneData *scene_data) override;

  /////////////////////////////////////////////////
  /// @brief  function to configure SceneConfig struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene, const SceneData *scene_data) override;
};
} // namespace steamrot
