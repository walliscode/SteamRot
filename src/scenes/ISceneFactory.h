/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FailInfo.h"
#include "GameContext.h"
#include "IEntityConfigurator.h"
#include "Scene.h"
#include "scene_types_generated.h"
#include <expected>
#include <memory>
#include <variant>
namespace steamrot {

class ISceneFactory {
protected:
  std::unique_ptr<IEntityConfigurator> m_entity_configurator{nullptr};

  const GameContext &m_game_context;

  SceneType scene_type{SceneType::SceneType_UNKNOWN};

  std::expected<std::unique_ptr<Scene>, FailInfo> CreateSceneByType();

public:
  ISceneFactory(const GameContext &game_context);

  // virtual destructor
  virtual ~ISceneFactory() = default;

  std::expected<std::unique_ptr<Scene>, FailInfo> CreateScene();

  std::expected<std::monostate, FailInfo> ConfigureSceneInfo(Scene &scene);

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene) = 0;

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene) = 0;

  // SceneState currently has no configuration needs, ready for future use
  // virtual std::expected<std::monostate, FailInfo>
  // ConfigureSceneState(Scene &scene) = 0;

  std::expected<std::monostate, FailInfo> ConfigureLogicMap(Scene &scene);
};
} // namespace steamrot
