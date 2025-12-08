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
#include "ISceneDataProvider.h"
#include "Scene.h"
#include "scene_types_generated.h"
#include <expected>
#include <memory>
#include <variant>
namespace steamrot {

class ISceneFactory {
private:
  std::unique_ptr<IEntityConfigurator> m_entity_configurator{nullptr};
  std::unique_ptr<ISceneDataProvider> m_scene_data_provider{nullptr};

  const GameContext &m_game_context;

  SceneType scene_type{SceneType::SceneType_UNKNOWN};

  std::expected<std::unique_ptr<Scene>, FailInfo> CreateSceneByType();

public:
  ISceneFactory(const GameContext &game_context);

  // virtual destructor
  virtual ~ISceneFactory() = default;

  std::expected<std::unique_ptr<Scene>, FailInfo> CreateScene();

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene) = 0;

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene) = 0;

  // SceneState currently has no configuration needs, ready for future use
  // virtual std::expected<std::monostate, FailInfo>
  // ConfigureSceneState(Scene &scene) = 0;
};
} // namespace steamrot
