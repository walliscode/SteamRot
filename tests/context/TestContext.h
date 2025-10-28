/////////////////////////////////////////////////
/// @file
/// @brief Declaration of TestContext object
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ArchetypeManager.h"
#include "EntityManager.h"
#include "EventHandler.h"
#include "GameContext.h"
#include "GameResources.h"
#include "SceneContext.h"
#include "SceneResources.h"
#include "containers.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
namespace steamrot::tests {
/////////////////////////////////////////////////
/// @class TestContext
/// @brief Contains instances of objects used for context in tests
///
/////////////////////////////////////////////////
/////////////////////////////////////////////////
/// @class TestContext
/// @brief 19H
///
/////////////////////////////////////////////////
class TestContext {

private:
  /////////////////////////////////////////////////
  /// @brief GameResources instance for tests
  /////////////////////////////////////////////////
  steamrot::GameResources game_resources;

  /////////////////////////////////////////////////
  /// @brief SceneResources instance for tests
  /////////////////////////////////////////////////
  steamrot::SceneResources scene_resources;

  /////////////////////////////////////////////////
  /// @brief EntityManager instance for tests
  /////////////////////////////////////////////////
  steamrot::EntityManager entity_manager;

  /////////////////////////////////////////////////
  /// @brief Mock GameContext instance for tests
  /////////////////////////////////////////////////
  std::unique_ptr<GameContext> game_context_ptr{nullptr};

  /////////////////////////////////////////////////
  /// @brief Mock SceneContext instance for tests with a test scene
  /////////////////////////////////////////////////
  std::unique_ptr<SceneContext> scene_context_for_test_scene{nullptr};

  /////////////////////////////////////////////////
  /// @brief Mock SceneContext instance for tests with a title scene
  /////////////////////////////////////////////////
  std::unique_ptr<SceneContext> scene_context_for_title_scene{nullptr};

  /////////////////////////////////////////////////
  /// @brief Mock SceneContext instance for tests with a crafting scene
  /////////////////////////////////////////////////
  std::unique_ptr<SceneContext> scene_context_for_crafting_scene{nullptr};

  /////////////////////////////////////////////////
  /// @brief Configure the GameContext instance
  /////////////////////////////////////////////////
  void ConfigureGameContext();

  /////////////////////////////////////////////////
  /// @brief Configure the Scene Context for the Test Scene
  /////////////////////////////////////////////////
  void ConfigureSceneContextForTestScene();

  /////////////////////////////////////////////////
  /// @brief Configure the Scene Context for the Title Scene
  /////////////////////////////////////////////////
  void ConfigureSceneContextForTitleScene();

  /////////////////////////////////////////////////
  /// @brief Configure the Scene Context for the Crafting Scene
  /////////////////////////////////////////////////
  void ConfigureSceneContextForCraftingScene();

public:
  TestContext(const SceneType scene_type = SceneType::SceneType_TEST);

  steamrot::GameContext &GetGameContext() const;

  const steamrot::SceneContext &GetSceneContextForTestScene() const;

  const steamrot::SceneContext &GetSceneContextForTitleScene() const;

  const steamrot::SceneContext &GetSceneContextForCraftingScene() const;
};
} // namespace steamrot::tests
