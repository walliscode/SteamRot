////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "CraftingScene.h"
#include "GameContext.h"
#include "Scene.h"
#include "SceneType.h"
#include "TitleScene.h"
#include "scenes_generated.h"
#include "uuid.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

namespace steamrot {

class SceneFactory {
private:
  const GameContext m_game_context;
  /**
   * @brief Creates a TitleScreen object with provided values
   *
   * @param scene_data Contains all Scene config data such as entities, actions
   * e.t.c.
   * @param id A uniue ID generated and provided by the SceneFactory
   */
  std::unique_ptr<TitleScene> CreateTitleScene(const SceneData *scene_data,
                                               const uuids::uuid &id);

  std::unique_ptr<CraftingScene>
  CreateCraftingScene(const SceneData *scene_data, const uuids::uuid &id);
  ////////////////////////////////////////////////////////////
  /// \brief create a uuid if none is in provided json data
  ///
  ////////////////////////////////////////////////////////////
  const uuids::uuid CreateUUID();

public:
  ////////////////////////////////////////////////////////////
  /// \brief Default constructor
  ///
  ////////////////////////////////////////////////////////////
  SceneFactory(const GameContext game_context);

  ////////////////////////////////////////////////////////////
  /// \brief gathers all scene creation methods
  ///
  ////////////////////////////////////////////////////////////
  std::unique_ptr<Scene> CreateScene(const SceneType &scene_type);
};
} // namespace steamrot
