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
    : m_resource_configurator(config) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ContextConfigurator::ConfigureGameResources(GameResources &resources) const {
  return m_resource_configurator.ConfigureGameResources(resources);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ContextConfigurator::ConfigureSceneResources(
    SceneResources &resources, const SceneType &scene_type) const {
  return m_resource_configurator.ConfigureSceneResources(resources, scene_type);
}

} // namespace steamrot
