////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "EntityConfigurationFactory.h"
#include "CUserInterface.h"
#include "EntityHelpers.h"
#include "containers.h"
#include "entities_generated.h"
#include "log_handler.h"
#include <iostream>
namespace steamrot {
////////////////////////////////////////////////////////////
EntityConfigurationFactory::EntityConfigurationFactory() {};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void EntityConfigurationFactory::ConfigureEntitiesFromDefaultData(
    components::containers::EntityMemoryPool &entity_memory_pool,
    const EntityCollection *entity_collection) {

  // check if entity_data is not null
  if (entity_collection->entities() == nullptr) {

    log_handler::ProcessLog(spdlog::level::level_enum::info,
                            log_handler::LogCode::kNoCode,
                            "Entity data is empty, please add Entitiy Data.");
    return;
  }
  std::cout << "Configuring " << entity_collection->entities()->size()
            << " entities from default data..." << std::endl;
  for (size_t i{0}; i < entity_collection->entities()->size(); ++i) {

    // get the entity data from the entities_data
    const steamrot::EntityData *entity_data =
        entity_collection->entities()->Get(i);

    // manually check each component type
    if (entity_data->c_user_interface()) {
      // pass the data through the CUserInterface component
      GetComponent<CUserInterface>(i, entity_memory_pool)
          .Configure(entity_data->c_user_interface());
      std::cout << "Configured CUserInterface for entity " << i << std::endl;
    }
    if (entity_data->c_grimoire_machina()) {
      // pass the data through the CGrimoireMachina component
      GetComponent<CGrimoireMachina>(i, entity_memory_pool)
          .Configure(entity_data->c_grimoire_machina());
      std::cout << "Configured CGrimoireMachina for entity " << i << std::endl;
    }
  }
};
} // namespace steamrot
