/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the abstract Scene class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameContext.h"
#include "SceneConfig.h"
#include "SceneInfo.h"
#include "SceneResources.h"
#include "SceneState.h"
#include <SFML/Graphics.hpp>
#include <memory>

#include <uuid.h>
typedef std::vector<std::shared_ptr<sf::Drawable>> SceneDrawables;

namespace steamrot {

/////////////////////////////////////////////////
/// @class Scene
/// @brief Abstract base class for all Scenes in the game.
///
/////////////////////////////////////////////////
class Scene {
  friend class SceneFactory;

protected:
  /////////////////////////////////////////////////
  /// @brief Identifying information about the Scene.
  /////////////////////////////////////////////////
  SceneInfo m_scene_info;

  /////////////////////////////////////////////////
  /// @brief Scene-level resources (managers, logic, render texture).
  /////////////////////////////////////////////////
  SceneResources m_scene_resources;

  /////////////////////////////////////////////////
  /// @brief Scene configuration loaded from data files.
  /////////////////////////////////////////////////
  SceneConfig m_scene_config;

  /////////////////////////////////////////////////
  /// @brief Scene runtime state (active flag, etc.).
  /////////////////////////////////////////////////
  SceneState m_scene_state;

  /////////////////////////////////////////////////
  /// @brief Constructor for Scene class.
  ///
  /// @param id Generated UUID for the Scene.
  /// @param game_context GameContext object passed down from the GameEngine.
  /////////////////////////////////////////////////
  Scene(const GameContext &game_context);

public:
  /////////////////////////////////////////////////
  /// @brief Destructor for Scene class.
  /////////////////////////////////////////////////
  virtual ~Scene() = default;

  /////////////////////////////////////////////////
  /// \brief function container for all movement related logic
  ///
  /////////////////////////////////////////////////
  virtual void sMovement() = 0;

  /////////////////////////////////////////////////
  /// @brief Function container for all collision related logic.
  /////////////////////////////////////////////////
  virtual void sCollision() = 0;

  /////////////////////////////////////////////////
  /// @brief Virtual container function for running all action related logic.
  /////////////////////////////////////////////////
  virtual void sAction() = 0;

  /////////////////////////////////////////////////
  /// @brief Virtual container function for running all rendering related logic.
  /////////////////////////////////////////////////
  virtual void sRender() = 0;

  /////////////////////////////////////////////////
  /// @brief Returns a reference to the RenderTexture of the Scene.
  /////////////////////////////////////////////////
  sf::RenderTexture &GetRenderTexture();

  /////////////////////////////////////////////////
  /// @brief Set the resize strategy for this scene.
  ///
  /// Replaces the current strategy.  Passing nullptr disables resize
  /// handling for this scene.
  ///
  /// @param strategy Shared pointer to the new resize strategy.
  /////////////////////////////////////////////////
  void SetResizeStrategy(std::shared_ptr<IResizeStrategy> strategy);

  /////////////////////////////////////////////////
  /// @brief Apply the scene's resize strategy to the render texture.
  ///
  /// Called by SceneManager::HandleResize() when the game window changes
  /// size.  If no strategy is set this method is a no-op.
  ///
  /// @param new_size New window dimensions in pixels.
  /////////////////////////////////////////////////
  void HandleResize(sf::Vector2u new_size);

  /////////////////////////////////////////////////
  /// @brief Returns the active state of the Scene.
  ///
  /// @return Boolean indicating if the Scene is active.
  /////////////////////////////////////////////////
  bool GetActive() const;

  /////////////////////////////////////////////////
  /// \brief change the active state of the Scene
  ///
  /////////////////////////////////////////////////
  void SetActive(bool active);

  /////////////////////////////////////////////////
  /// @brief Returns the SceneInfo of the Scene.
  /////////////////////////////////////////////////
  const SceneInfo GetSceneInfo() const;

  SceneInfo &GetSceneInfo();

  /////////////////////////////////////////////////
  /// @brief Return a read-only reference to the SceneResources of the Scene.
  ///
  /// @return A const reference to the SceneResources.
  /////////////////////////////////////////////////
  const SceneResources &GetSceneResources() const;

  /////////////////////////////////////////////////
  /// @brief Create and return a SceneContext object for the Scene.
  ///
  /// This contains all the references needed for logic processing.
  /// @return a Scene specific SceneContext object
  /////////////////////////////////////////////////
  SceneContext GetSceneContext();

  EntityManager &GetEntityManager();

#ifdef DEBUG
  const EntityMemoryPool &GetEntityMemoryPool() const;

  const std::unordered_map<ArchetypeID, Archetype> &GetArchetypes() const;
#endif
};

} // namespace steamrot
