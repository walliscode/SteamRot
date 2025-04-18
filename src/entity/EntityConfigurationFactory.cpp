////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "EntityConfigurationFactory.h"
#include <iostream>

////////////////////////////////////////////////////////////
EntityConfigurationFactory::EntityConfigurationFactory(
    const std::string &scene_id)
    : m_scene_id(scene_id) {}

////////////////////////////////////////////////////////////
void EntityConfigurationFactory::ConfigureEntities(

    const std::string &config_method,
    std::unique_ptr<EntityMemoryPool> &entity_memory_pool) {

  // pick the configuration method based on the provided key
  if (config_method == "json") {
    ConfigureFromJSON();
  }
  // final statement to handle non-matching keys
  else {
    std::cerr << "Invalid configuration method: " << config_method << std::endl;
  }
}

////////////////////////////////////////////////////////////
void EntityConfigurationFactory::ConfigureFromJSON() {};
