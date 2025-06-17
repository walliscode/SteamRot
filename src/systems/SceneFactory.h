////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "DataManager.h"
#include "Scene.h"
#include "SceneType.h"
#include "TitleScene.h"
#include "scenes_generated.h"
#include "uuid.h"
#include <memory>

namespace steamrot {

class SceneFactory {
private:
  ////////////////////////////////////////////////////////////
  /// \brief instance of DataManager, keep this here as long as DataManager is
  /// stateless
  ///
  ////////////////////////////////////////////////////////////
  DataManager m_data_manager;

  /**
   * @brief Creates a TitleScreen object with provided values
   *
   * @param scene_data Contains all Scene config data such as entities, actions
   * e.t.c.
   * @param id A uniue ID generated and provided by the SceneFactory
   */
  std::unique_ptr<TitleScene>
  CreateTitleScene(const SceneData *scene_data, const uuids::uuid &id,
                   const AssetManager &asset_manager);

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
  SceneFactory();
  ;
  ////////////////////////////////////////////////////////////
  /// \brief gathers all scene creation methods
  ///
  ////////////////////////////////////////////////////////////
  std::unique_ptr<Scene> CreateScene(const SceneType &scene_type,
                                     const AssetManager &asset_manager);
};
} // namespace steamrot
