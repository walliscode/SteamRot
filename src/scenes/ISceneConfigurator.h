/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ISceneConfigurator interface.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "FailInfo.h"
#include "Scene.h"
#include <expected>
#include <variant>
namespace steamrot {
/////////////////////////////////////////////////
/// @class ISceneConfigurator
/// @brief Provides an interface for configuring different aspects of a scene.
///
/// Primarily designed to provide a strategy pattern for the SceneFactory
/////////////////////////////////////////////////
class ISceneConfigurator {

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for ISceneConfigurator
  /////////////////////////////////////////////////
  ISceneConfigurator() = default;

  /////////////////////////////////////////////////
  /// @brief Wrapper function for all scene configuration
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureScene(Scene &scene);

  /////////////////////////////////////////////////
  /// @brief Virtual function to configure SceneInfo struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene) = 0;

  /////////////////////////////////////////////////
  /// @brief Virtual function to configure SceneResources struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene) = 0;

  /////////////////////////////////////////////////
  /// @brief Virtual function to configure SceneConfig struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure the logic map for the scene
  ///
  /// This should be set in stone, not configurable by data or the user.
  /// @param scene  Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureLogicMap(Scene &scene);
};
} // namespace steamrot
