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
#include "scene_data_generated.h"

namespace steamrot {

class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {

private:
  SceneDataFbs *m_scene_data = nullptr;

public:
  FlatbuffersDefaultSceneConfigurator();

  /////////////////////////////////////////////////
  /// @brief  function to configure SceneInfo struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene) override;

  /////////////////////////////////////////////////
  /// @brief  function to configure SceneResources struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene) override;

  /////////////////////////////////////////////////
  /// @brief  function to configure SceneConfig struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene) override;
};
} // namespace steamrot
