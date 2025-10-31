////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of TestFixture class for resource-based testing.
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "EntityManager.h"
#include "GameContext.h"
#include "GameResources.h"
#include "SceneContext.h"
#include "SceneResources.h"
#include "entities_generated.h"
#include "scene_change_packet_generated.h"
#include <memory>

namespace steamrot::tests {

////////////////////////////////////////////////////////////
/// @brief Test infrastructure for generating resources and contexts.
///
/// TestFixture provides a simplified way to create test resources and
/// contexts without using builders. It owns GameResources, SceneResources,
/// and EntityManager, and provides contexts on demand.
///
/// This replaces the builder-based approach with direct resource ownership
/// and configuration.
////////////////////////////////////////////////////////////
class TestFixture {
private:
  /////////////////////////////////////////////////
  /// @brief Preconfigured PathProvider for test environment.
  /////////////////////////////////////////////////
  PathProvider path_provider{EnvironmentType::Test};

  ////////////////////////////////////////////////////////////
  /// @brief Game-level resources owned by the fixture.
  ////////////////////////////////////////////////////////////
  GameResources m_game_resources;

  ////////////////////////////////////////////////////////////
  /// @brief Scene-level resources owned by the fixture.
  ////////////////////////////////////////////////////////////
  SceneResources m_scene_resources;

  ////////////////////////////////////////////////////////////
  /// @brief EntityManager needed for SceneContext.
  ////////////////////////////////////////////////////////////
  EntityManager m_entity_manager;

  ////////////////////////////////////////////////////////////
  /// @brief GameContext instance (created on demand).
  ////////////////////////////////////////////////////////////
  std::unique_ptr<GameContext> m_game_context;

  ////////////////////////////////////////////////////////////
  /// @brief SceneContext instance (created on demand).
  ////////////////////////////////////////////////////////////
  std::unique_ptr<SceneContext> m_scene_context;

  ////////////////////////////////////////////////////////////
  /// @brief Scene type for this fixture.
  ////////////////////////////////////////////////////////////
  SceneType m_scene_type;

  ////////////////////////////////////////////////////////////
  /// @brief Configure GameResources for testing.
  ///
  /// Uses the reusable resources::ConfigureGameResources
  /// function and additionally loads assets and sets environment type.
  ////////////////////////////////////////////////////////////
  void ConfigureGameResourcesForTest();

  ////////////////////////////////////////////////////////////
  /// @brief Configure scene-level resources including EntityManager.
  ///
  /// Configures SceneResources and EntityManager. If entity_collection is
  /// provided, entities are configured from it; otherwise, entities are
  /// loaded from default scene data.
  ///
  /// @param scene_type Scene type to configure for
  /// @param entity_collection Optional entity collection for custom entity data
  ////////////////////////////////////////////////////////////
  void ConfigureSceneLevelResources(const SceneType &scene_type,
                                    const EntityCollection *entity_collection = nullptr);

public:
  ////////////////////////////////////////////////////////////
  /// @brief Constructor for TestFixture.
  ///
  /// @param scene_type Scene type to configure (default: TEST)
  ////////////////////////////////////////////////////////////
  explicit TestFixture(const SceneType &scene_type = SceneType::SceneType_TEST);

  /////////////////////////////////////////////////
  /// @brief Set up the test fixture by configuring resources.
  ///
  /// Configures game resources and scene-level resources (including
  /// EntityManager). If entity_collection is provided, entities are
  /// configured from it instead of loading default scene data.
  ///
  /// @param entity_collection Optional entity collection for custom entity data
  /////////////////////////////////////////////////
  void Intialize(const EntityCollection *entity_collection = nullptr);

  ////////////////////////////////////////////////////////////
  /// @brief Get reference to owned GameResources.
  ///
  /// @return Reference to GameResources
  ////////////////////////////////////////////////////////////
  GameResources &GetGameResources();

  ////////////////////////////////////////////////////////////
  /// @brief Get reference to owned SceneResources.
  ///
  /// @return Reference to SceneResources
  ////////////////////////////////////////////////////////////
  SceneResources &GetSceneResources();

  ////////////////////////////////////////////////////////////
  /// @brief Get reference to owned EntityManager.
  ///
  /// @return Reference to EntityManager
  ////////////////////////////////////////////////////////////
  EntityManager &GetEntityManager();

  ////////////////////////////////////////////////////////////
  /// @brief Get reference to GameContext (created on first call).
  ///
  /// @return Reference to GameContext
  ////////////////////////////////////////////////////////////
  GameContext &GetGameContext();

  ////////////////////////////////////////////////////////////
  /// @brief Get reference to SceneContext (created on first call).
  ///
  /// @return Reference to SceneContext
  ////////////////////////////////////////////////////////////
  SceneContext &GetSceneContext();
};

} // namespace steamrot::tests
