////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ActionManager.h"
#include "BaseLogic.h"
#include "EntityManager.h"
#include "GameContext.h"
#include "LogicFactory.h"
#include "global_constants.h"
#include "logics_generated.h"
#include "scenes_generated.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <nlohmann/json.hpp>

#include <unordered_map>
#include <uuid.h>
typedef std::vector<std::shared_ptr<sf::Drawable>> SceneDrawables;

namespace steamrot {

class Scene {
  friend class SceneFactory;

protected:
  ////////////////////////////////////////////////////////////
  // Member: Entity Manager instance
  ////////////////////////////////////////////////////////////
  EntityManager m_entity_manager;
  ////////////////////////////////////////////////////////////
  // Member: Action Manager instance
  ////////////////////////////////////////////////////////////
  ActionManager m_action_manager;

  /////////////////////////////////////////////////
  /// @brief Instance of LogicFactory class. Create Logic objects for Scene to
  /// store.
  /////////////////////////////////////////////////
  LogicFactory m_logic_factory;

  /////////////////////////////////////////////////
  /// @brief GameContext object passed down from the GameEngine.
  /////////////////////////////////////////////////
  const GameContext m_game_context;

  /////////////////////////////////////////////////
  /// @brief Map of all logic objects needed by the Scene.
  /////////////////////////////////////////////////
  std::unordered_map<LogicType, std::vector<std::unique_ptr<BaseLogic>>>
      m_logics;

  ////////////////////////////////////////////////////////////
  // Member: unique id generated for each Scene instance
  ////////////////////////////////////////////////////////////
  uuids::uuid m_id;

  /////////////////////////////////////////////////
  /// @brief RenderTexture for the Scene instance.
  /////////////////////////////////////////////////
  sf::RenderTexture m_render_texture{kWindowSize};

  bool m_paused = false;
  bool m_active = true;

  bool m_interactable = false;
  size_t m_current_frame = 0;

  /**
   * @brief Scene constructor that initilizes the Scene with  flatbuffers data
   *
   * @param pool_size Size of the EntityMemoryPool for the Scene
   * @param scene_data Any config/data needed to initialize the Scene
   * @param id Unique identifier for the Scene
   */
  Scene(const size_t &pool_size, const SceneData *scene_data,
        const uuids::uuid &id, const GameContext game_context);

public:
  /**
   * @brief Virtual destructor for the Scene class, needed for proper cleanup
   */
  virtual ~Scene() = default;
  ////////////////////////////////////////////////////////////
  /// \brief function container for all movement related logic
  ///
  ////////////////////////////////////////////////////////////
  virtual void sMovement() = 0;

  /////////////////////////////////////////////////
  /// @brief Function container for all collision related logic.
  /////////////////////////////////////////////////
  virtual void sCollision() = 0;

  /**
   * @brief Function container for all rendering related logic.
   */
  virtual void sRender() = 0;
  /**
   * @brief Return the RenderTexture for the Scene instance.
   */
  sf::RenderTexture &GetRenderTexture();

  ////////////////////////////////////////////////////////////
  /// \brief Inidcicates if the Scenes is active or not
  ///
  ////////////////////////////////////////////////////////////
  bool GetActive() const;

  ////////////////////////////////////////////////////////////
  /// \brief change the active state of the Scene
  ///
  ////////////////////////////////////////////////////////////
  void SetActive(bool active);

  ////////////////////////////////////////////////////////////
  /// \brief return Scene id
  ////////////////////////////////////////////////////////////
  const uuids::uuid GetSceneID();
};

} // namespace steamrot
