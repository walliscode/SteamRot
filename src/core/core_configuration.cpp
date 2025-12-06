/////////////////////////////////////////////////
/// @file
/// @brief Free functions for configuring core objects from native data structs.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "core_configuration.h"

namespace steamrot {
namespace core {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineResources(EngineResources &engine_resources,
                         const EngineResourcesConfigData &config_data) {
  // Create the window with configured settings (SFML 3.0 API)
  sf::Vector2u window_size(config_data.window_width, config_data.window_height);

  std::string window_title =
      config_data.window_title.empty() ? "SteamRot" : config_data.window_title;

  engine_resources.game_window.create(sf::VideoMode(window_size), window_title);

  // Set framerate limit
  engine_resources.game_window.setFramerateLimit(config_data.framerate_limit);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneCore(SceneCore &scene_core, const SceneCoreData &core_data) {
  // Create the render texture with configured dimensions (SFML 3.0 API)
  sf::Vector2u texture_size(core_data.render_texture_width,
                            core_data.render_texture_height);
  scene_core.scene_texture = sf::RenderTexture(texture_size);

  return std::monostate{};
}

} // namespace core
} // namespace steamrot
