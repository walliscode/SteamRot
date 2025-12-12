/////////////////////////////////////////////////
/// @file
/// @brief Free functions for configuring core objects from native data structs.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "engine_configuration.h"

namespace steamrot {
namespace engine {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineResources(EngineResources &engine_resources,
                         const EngineResourcesConfig &config_data) {
  // Create the window with configured settings (SFML 3.0 API)
  sf::Vector2u window_size(config_data.window_width, config_data.window_height);

  std::string window_title =
      config_data.window_title.empty() ? "SteamRot" : config_data.window_title;

  engine_resources.game_window.create(sf::VideoMode(window_size), window_title);

  // Set framerate limit
  engine_resources.game_window.setFramerateLimit(config_data.framerate_limit);

  return std::monostate{};
}

} // namespace engine
} // namespace steamrot
