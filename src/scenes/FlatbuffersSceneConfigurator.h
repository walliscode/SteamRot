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

public:
  /////////////////////////////////////////////////
  /// @brief default constructor
  /////////////////////////////////////////////////
  FlatbuffersSceneConfigurator() = default;

  /////////////////////////////////////////////////
  /// @brief Virtual default destructor
  /////////////////////////////////////////////////
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

  /////////////////////////////////////////////////
  /// @brief  function to configure entities in the scene
  ///
  /// @param scene Scene whose entities should be configured
  /// @param scene_data SceneData containing entity configuration
  /// @return std::expected with monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEntities(Scene &scene, const SceneData *scene_data) override;
};
} // namespace steamrot
