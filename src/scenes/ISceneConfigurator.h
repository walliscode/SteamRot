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
  ISceneConfigurator() = default;

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene) = 0;

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene) = 0;

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene) = 0;

  std::expected<std::monostate, FailInfo> ConfigureLogicMap(Scene &scene);
};
} // namespace steamrot
