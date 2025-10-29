////////////////////////////////////////////////////////////
/// @file
/// @brief Free functions for configuring resources from FlatBuffers data.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "resources_configuration.h"

namespace steamrot {
namespace resources {

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureGameResources(GameResources &resources,
                       const GameResourcesData *game_data) {
  if (!game_data) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "GameResourcesData is null"});
  }

  // Create the window with configured settings (SFML 3.0 API)
  sf::Vector2u window_size(game_data->window_width(),
                           game_data->window_height());

  std::string window_title = "SteamRot"; // Default title
  if (game_data->window_title()) {
    window_title = game_data->window_title()->str();
  }

  resources.game_window.create(sf::VideoMode(window_size), window_title);

  // Set framerate limit
  resources.game_window.setFramerateLimit(game_data->framerate_limit());

  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneResources(SceneResources &resources,
                        const SceneResourcesData *scene_data) {
  
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
  resources.scene_texture = sf::RenderTexture(texture_size);

  return std::monostate{};
}

} // namespace resources
} // namespace steamrot
