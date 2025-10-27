/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ResourceConfigurator class
/////////////////////////////////////////////////

#include "ResourceConfigurator.h"
#include "FailInfo.h"
#include "global_constants.h"

namespace steamrot {

/////////////////////////////////////////////////
ResourceConfigurator::ResourceConfigurator(const ContextData *config)
    : m_config_data(config) {}

/////////////////////////////////////////////////
std::expected<EnvironmentType, FailInfo>
ResourceConfigurator::ParseEnvironmentType(const std::string &type_str) const {
  if (type_str == "Test") {
    return EnvironmentType::Test;
  } else if (type_str == "Production") {
    return EnvironmentType::Production;
  } else if (type_str == "None") {
    return EnvironmentType::None;
  }

  return std::unexpected(FailInfo{FailMode::InvalidData,
                                  "Invalid environment type: " + type_str});
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ResourceConfigurator::ConfigureGameResources(GameResources &resources) const {
  if (!m_config_data) {
    return std::unexpected(
        FailInfo{FailMode::NullData, "ContextData is null"});
  }

  const auto *game_config = m_config_data->game_context();
  if (!game_config) {
    return std::unexpected(
        FailInfo{FailMode::NullData, "GameContextConfig is null"});
  }

  // Parse environment type
  auto env_type_result =
      ParseEnvironmentType(game_config->environment_type()->str());
  if (!env_type_result.has_value()) {
    return std::unexpected(env_type_result.error());
  }
  resources.env_type = env_type_result.value();

  // Configure window
  const uint32_t width = game_config->window_width();
  const uint32_t height = game_config->window_height();
  const std::string title =
      game_config->window_title() ? game_config->window_title()->str()
                                  : "SteamRot";

  resources.game_window.create(sf::VideoMode({width, height}), title);

  // Set framerate limit
  resources.game_window.setFramerateLimit(game_config->framerate_limit());

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ResourceConfigurator::ConfigureSceneResources(
    SceneResources &resources, const SceneType &scene_type) const {
  if (!m_config_data) {
    return std::unexpected(
        FailInfo{FailMode::NullData, "ContextData is null"});
  }

  const auto *scene_contexts = m_config_data->scene_contexts();
  if (!scene_contexts) {
    return std::unexpected(
        FailInfo{FailMode::NullData, "SceneContexts vector is null"});
  }

  // Find the configuration for this scene type
  const SceneContextConfig *scene_config = nullptr;
  for (const auto *config : *scene_contexts) {
    if (config->scene_type() == scene_type) {
      scene_config = config;
      break;
    }
  }

  if (!scene_config) {
    return std::unexpected(FailInfo{
        FailMode::NotFound,
        "No configuration found for scene type: " + std::to_string(static_cast<int>(scene_type))});
  }

  // Configure entity pool size
  const uint32_t pool_size = scene_config->entity_pool_size();
  resources.scene_entities = EntityMemoryPool(pool_size);
  
  // Recreate archetype manager with new scene_entities
  resources.archetype_manager = ArchetypeManager(resources.scene_entities);

  // Configure render texture
  const uint32_t tex_width = scene_config->render_texture_width();
  const uint32_t tex_height = scene_config->render_texture_height();
  
  if (!resources.scene_texture.create({tex_width, tex_height})) {
    return std::unexpected(
        FailInfo{FailMode::ResourceCreationFailure,
                 "Failed to create render texture with dimensions " +
                     std::to_string(tex_width) + "x" + std::to_string(tex_height)});
  }

  return std::monostate{};
}

} // namespace steamrot
