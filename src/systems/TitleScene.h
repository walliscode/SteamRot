/**
 * @file
 * @brief TitleScreen class for display entry point of the game.
 */

#pragma once

#include "Scene.h"
#include "scenes_generated.h"
namespace steamrot {

/**
 * @class TitleScreen
 * @brief Contains information and logic for the title screen of the game.
 *
 */
class TitleScene : public Scene {
  friend class SceneFactory;

public:
  void sRender() override;

private:
  /**
   * @brief Constructor for TitleScreen
   *
   * @param pool_size EntityMemoryPool size for the scene
   * @param config_data JSON configuration data for the scene
   * @param id Unique identifier for the scene
   */
  TitleScene(const size_t pool_size, const SceneData *scene_data,
             const uuids::uuid &id);

  ////////////////////////////////////////////////////////////
  /// \brief call movement Logic for TitleScene
  ////////////////////////////////////////////////////////////
  void sMovement() override;
};
} // namespace steamrot
