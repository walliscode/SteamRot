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
ConfigureGameCore(GameCore &game_core, const EngineCoreData &core_data) {
  // Create the window with configured settings (SFML 3.0 API)
  sf::Vector2u window_size(core_data.window_width, core_data.window_height);

  std::string window_title =
      core_data.window_title.empty() ? "SteamRot" : core_data.window_title;

  game_core.game_window.create(sf::VideoMode(window_size), window_title);

  // Set framerate limit
  game_core.game_window.setFramerateLimit(core_data.framerate_limit);

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
