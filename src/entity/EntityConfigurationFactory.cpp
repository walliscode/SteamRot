////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "EntityConfigurationFactory.h"
#include "containers.h"
#include "log_handler.h"
#include <memory>
#include <tuple>
namespace steamrot {
////////////////////////////////////////////////////////////
EntityConfigurationFactory::EntityConfigurationFactory(
    const std::string &scene_id)
    : m_scene_id(scene_id) {}

////////////////////////////////////////////////////////////
void EntityConfigurationFactory::ConfigureEntities(
    std::unique_ptr<components::containers::EntityMemoryPool> pool,
    json &data) {

  // check if data contains "entities" key
  if (!data.contains("entities")) {
    log_handler::ProcessLog(spdlog::level::level_enum::err,
                            log_handler::LogCode::kInvalidJSONKey,
                            "entities key not found in JSON data.");
  }

  json entities{data["entities"]};

  for (size_t i{0}; i < entities.size(); ++i) {

    json entity_data{entities[i]};
    // iterate over the EntityMemoryPool
    std::apply(
        [i, entity_data](auto &...vecs) {
          (
              [&] {
                // get the component data from the given index
                auto component = vecs[i];

                // get the component name
                std::string component_name = component.Name();

                // check if the component name exists in the entity data
                if (!entity_data.contains(component_name)) {
                  log_handler::ProcessLog(
                      spdlog::level::level_enum::err,
                      log_handler::LogCode::kInvalidJSONKey,
                      fmt::format("Component {} not found in entity data.",
                                  component_name));
                }
                // get the component name from the component type
                json component_data = entity_data[component_name];

                // configure the component with the data
                component.Configure(component_data);
              }(),
              ...);
        },
        *pool);
  }
};

} // namespace steamrot
