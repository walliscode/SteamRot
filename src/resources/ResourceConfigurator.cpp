////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ResourceConfigurator class.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ResourceConfigurator.h"

namespace steamrot {

////////////////////////////////////////////////////////////
ResourceConfigurator::ResourceConfigurator(const GameResourcesData *game_data)
    : m_game_resources_data(game_data) {}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ResourceConfigurator::ConfigureGameResources(GameResources &resources) const {
  if (!m_game_resources_data) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "GameResourcesData is null"});
  }

  const GameResourcesData *game_config = m_game_resources_data;

  // Create the window with configured settings (SFML 3.0 API)
  sf::Vector2u window_size(game_config->window_width(),
                           game_config->window_height());

  std::string window_title = "SteamRot"; // Default title
  if (game_config->window_title()) {
    window_title = game_config->window_title()->str();
  }

  resources.game_window.create(sf::VideoMode(window_size), window_title);

  // Set framerate limit
  resources.game_window.setFramerateLimit(game_config->framerate_limit());

  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ResourceConfigurator::ConfigureSceneResources(
    SceneResources &resources, const SceneResourcesData *scene_data) const {
  
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

} // namespace steamrot
