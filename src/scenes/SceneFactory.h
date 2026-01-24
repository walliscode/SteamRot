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
#include "Scene.h"
#include "SceneData.h"
#include "scene_types_generated.h"
#include <expected>
#include <memory>

namespace steamrot {

class SceneFactory {
protected:
  /////////////////////////////////////////////////
  /// @brief Refrerence to the game context object (global resources)
  /////////////////////////////////////////////////
  const GameContext &m_game_context;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for SceneFactory
  ///
  /// @param game_context GameContext object providing access to global
  /// resources
  /////////////////////////////////////////////////
  SceneFactory(const GameContext &game_context);

  /////////////////////////////////////////////////
  /// @brief Use the factory to create and configure a scene
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateEmptyScene(const SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Create and configure a scene from SceneData
  ///
  /// @param scene_data Data containing scene config and entity importer
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromSceneData(const SceneData &scene_data);

  /////////////////////////////////////////////////
  /// @brief Create and configure a scene from default data
  ///
  /// @param scene_type SceneType enum value representing the type of scene to
  /// create
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromDefault(const SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Wrapper function for all scene configuration
  ///
  /// @param scene Scene to configure
  /// @param scene_data SceneData containing configuration data
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData &scene_data);

  /////////////////////////////////////////////////
  /// @brief Configure SceneInfo struct
  ///
  /// @param scene Scene to configure
  /// @param scene_data SceneData containing scene info
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneData &scene_data);

  /////////////////////////////////////////////////
  /// @brief Configure SceneResources struct
  ///
  /// @param scene Scene to configure
  /// @param scene_data SceneData containing scene resources config
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneData &scene_data);

  /////////////////////////////////////////////////
  /// @brief Configure SceneConfig struct
  ///
  /// @param scene Scene to configure
  /// @param scene_data SceneData containing scene config
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene, const SceneData &scene_data);

  /////////////////////////////////////////////////
  /// @brief Import entities into the scene from the entity importer
  ///
  /// @param scene Scene to import entities into
  /// @param scene_data SceneData containing entity importer
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ImportEntities(Scene &scene, const SceneData &scene_data);

  /////////////////////////////////////////////////
  /// @brief Pass AssetConfig object to the AssetManager
  ///
  /// @param scene Scene from which to get AssetManager ref
  /// @param scene_data Pointer to SceneData containing AssetConfig
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  PassAssetConfig(Scene &scene, const SceneData &scene_data);

  /////////////////////////////////////////////////
  /// @brief Configure the logic map for the scene
  ///
  /// This should be set in stone, not configurable by data or the user.
  /// @param scene  Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureLogicMap(Scene &scene);
};
} // namespace steamrot
