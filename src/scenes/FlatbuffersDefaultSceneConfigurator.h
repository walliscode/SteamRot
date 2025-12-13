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
#include "FlatbuffersDataLoader.h"
#include "ISceneConfigurator.h"
#include "scene_types_generated.h"

namespace steamrot {

class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {

private:
  FlatbuffersDataLoader m_data_loader;

public:
  FlatbuffersDefaultSceneConfigurator() = default;

  ~FlatbuffersDefaultSceneConfigurator() override = default;

  /////////////////////////////////////////////////
  /// @brief  function to configure SceneInfo struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneType scene_type) override;

  /////////////////////////////////////////////////
  /// @brief  function to configure SceneResources struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneType scene_type) override;

  /////////////////////////////////////////////////
  /// @brief  function to configure SceneConfig struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene, const SceneType scene_type) override;
};
} // namespace steamrot
