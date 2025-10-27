/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ContextConfigurator class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ContextConfigurator.h"

namespace steamrot {

/////////////////////////////////////////////////
ContextConfigurator::ContextConfigurator(const ContextData *config)
    : m_config_data(config) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ContextConfigurator::ConfigureGameResources(GameResources &resources) const {
  ResourceConfigurator resource_configurator(m_config_data);
  return resource_configurator.ConfigureGameResources(resources);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ContextConfigurator::ConfigureSceneResources(SceneResources &resources,
                                              const SceneType &scene_type) const {
  ResourceConfigurator resource_configurator(m_config_data);
  return resource_configurator.ConfigureSceneResources(resources, scene_type);
}

} // namespace steamrot
