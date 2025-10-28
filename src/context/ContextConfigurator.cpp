/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "ContextConfigurator.h"
#include "FlatbuffersDataLoader.h"
#include "ResourceConfigurator.h"

namespace steamrot {

/////////////////////////////////////////////////
ContextConfigurator::ContextConfigurator(const ContextData *config)
    : m_context_data(config) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ContextConfigurator::ConfigureGameResources(GameResources &resources) const {
  // Load GameResourcesData from file
  FlatbuffersDataLoader loader;
  auto game_resources_result = loader.ProvideGameResourcesData();
  if (!game_resources_result.has_value()) {
    return std::unexpected(game_resources_result.error());
  }

  // Create ResourceConfigurator and configure
  ResourceConfigurator resource_configurator(game_resources_result.value());
  return resource_configurator.ConfigureGameResources(resources);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ContextConfigurator::ConfigureSceneResources(
    SceneResources &resources, const SceneType &scene_type) const {
  // Load SceneResourcesData from scene file
  FlatbuffersDataLoader loader;
  auto game_resources_result = loader.ProvideGameResourcesData();
  if (!game_resources_result.has_value()) {
    return std::unexpected(game_resources_result.error());
  }

  auto scene_resources_result = loader.ProvideSceneResourcesData(scene_type);
  if (!scene_resources_result.has_value()) {
    return std::unexpected(scene_resources_result.error());
  }

  // Create ResourceConfigurator and configure
  // scene_resources_result.value() can be nullptr if not configured in scene file
  ResourceConfigurator resource_configurator(game_resources_result.value());
  return resource_configurator.ConfigureSceneResources(resources,
                                                       scene_resources_result.value());
}

} // namespace steamrot
