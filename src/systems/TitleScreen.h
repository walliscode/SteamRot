/**
 * @file
 * @brief TitleScreen class for display entry point of the game.
 */

#pragma once

#include "Scene.h"

namespace steamrot {

/**
 * @class TitleScreen
 * @brief Contains information and logic for the title screen of the game.
 *
 */
class TitleScreen : public Scene {
  friend class SceneFactory;

private:
  /**
   * @brief Constructor for TitleScreen
   *
   * @param pool_size EntityMemoryPool size for the scene
   * @param config_data JSON configuration data for the scene
   * @param id Unique identifier for the scene
   */
  TitleScreen(size_t pool_size, const json &config_data, const uuids::uuid &id);
};
} // namespace steamrot
