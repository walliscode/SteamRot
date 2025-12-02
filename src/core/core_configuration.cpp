/////////////////////////////////////////////////
/// @file
/// @brief Free functions for configuring core objects from FlatBuffers data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "core_configuration.h"

namespace steamrot {
namespace core {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureGameCore(GameCore &game_core, const EngineCoreData *core_data) {
  if (!core_data) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "EngineCoreData is null"});
  }

  // Create the window with configured settings (SFML 3.0 API)
  sf::Vector2u window_size(core_data->window_width(),
                           core_data->window_height());

  std::string window_title = "SteamRot"; // Default title
  if (core_data->window_title()) {
    window_title = core_data->window_title()->str();
  }

  game_core.game_window.create(sf::VideoMode(window_size), window_title);

  // Set framerate limit
  game_core.game_window.setFramerateLimit(core_data->framerate_limit());

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneCore(SceneCore &scene_core, const SceneCoreData *scene_data) {

  // Default dimensions
  uint32_t texture_width = 800;
  uint32_t texture_height = 600;

  // Use configured dimensions if provided
  if (scene_data) {
    texture_width = scene_data->render_texture_width();
    texture_height = scene_data->render_texture_height();
  }

  // Create the render texture with configured or default dimensions (SFML 3.0 API)
  sf::Vector2u texture_size(texture_width, texture_height);
  scene_core.scene_texture = sf::RenderTexture(texture_size);

  return std::monostate{};
}

} // namespace core
} // namespace steamrot
