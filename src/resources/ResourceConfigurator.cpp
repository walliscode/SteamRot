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
ResourceConfigurator::ResourceConfigurator(
    const GameResourcesData *game_data,
    const SceneResourcesCollection *scene_data)
    : m_game_resources_data(game_data), m_scene_resources_data(scene_data) {}

////////////////////////////////////////////////////////////
std::expected<EnvironmentType, FailInfo>
ResourceConfigurator::ParseEnvironmentType(const std::string &type_str) const {
  if (type_str == "None") {
    return EnvironmentType::None;
  } else if (type_str == "Test") {
    return EnvironmentType::Test;
  } else if (type_str == "Production") {
    return EnvironmentType::Production;
  } else {
    return std::unexpected(FailInfo{FailMode::NonExistentEnumValue,
                                    "Unknown environment type: " + type_str});
  }
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ResourceConfigurator::ConfigureGameResources(GameResources &resources) const {
  if (!m_game_resources_data) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "GameResourcesData is null"});
  }

  const GameResourcesData *game_config = m_game_resources_data;

  // Parse and set environment type if provided
  if (game_config->environment_type()) {
    auto env_type_result =
        ParseEnvironmentType(game_config->environment_type()->str());
    if (!env_type_result.has_value()) {
      return std::unexpected(env_type_result.error());
    }
    resources.env_type = env_type_result.value();
  }

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
    SceneResources &resources, const SceneType &scene_type) const {
  if (!m_scene_resources_data) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "SceneResourcesCollection is null"});
  }

  if (!m_scene_resources_data->scenes()) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField, "Scene resources are missing"});
  }

  // Find the configuration for the requested scene type
  const SceneResourcesData *scene_config = nullptr;
  for (const auto *config : *m_scene_resources_data->scenes()) {
    if (config && config->scene_type() == scene_type) {
      scene_config = config;
      break;
    }
  }

  if (!scene_config) {
    return std::unexpected(
        FailInfo{FailMode::SceneTypeNotFound,
                 "Scene configuration not found for requested scene type"});
  }

  // Create the render texture with configured dimensions (SFML 3.0 API)
  sf::Vector2u texture_size(scene_config->render_texture_width(),
                            scene_config->render_texture_height());

  resources.scene_texture = sf::RenderTexture(texture_size);

  return std::monostate{};
}

} // namespace steamrot
