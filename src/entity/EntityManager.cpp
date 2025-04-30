////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "EntityManager.h"
#include "EntityMemoryPool.h"

#include <memory>

using json = nlohmann::json;
using namespace magic_enum::bitwise_operators;

////////////////////////////////////////////////////////////
EntityManager::EntityManager(const size_t &pool_size,
                             const std::string &scene_name)
    : m_entity_configuration_factory(scene_name),
      m_pool(std::make_unique<EntityMemoryPool>(pool_size)) {

      };

////////////////////////////////////////////////////////////
void EntityManager::ConfigureEntities(const std::string &config_method) {

  m_entity_configuration_factory.ConfigureEntities(config_method, m_pool);
}

////////////////////////////////////////////////////////////
size_t EntityManager::AddEntity() {

  size_t new_entity_id = (*m_pool).getNextEntityIndex();

  RefreshEntity(*(*m_pool).getData(), new_entity_id);

  std::vector<CMeta> meta_data =
      std::get<std::vector<CMeta>>(*(m_pool->getData()));

  // switch on the entity
  meta_data[new_entity_id].m_entity_active = true;
  m_entities_to_add.push_back(new_entity_id);

  return new_entity_id;
}

////////////////////////////////////////////////////////////
void EntityManager::RemoveEntity(size_t entity_index) {

  m_entities_to_remove.push_back(entity_index);
}

////////////////////////////////////////////////////////////
void EntityManager::UpdateWaitingRooms() {
  // Entity removal/deleteion
  // for each entity to remove, deactivate the CMeta component
  for (const size_t &entity_index : m_entities_to_remove) {

    std::get<std::vector<CMeta>>(*(m_pool->getData()))[entity_index]
        .m_entity_active = false;

    // get iterator to entity to remove
    auto to_remove =
        std::find(m_entities.begin(), m_entities.end(), entity_index);

    // m_archetypeManager.clearEntity(
    //     entityIndex, {});

    // remove from vector of current entitie indices
    m_entities.erase(to_remove);
  }
  m_entities_to_remove.clear(); // clear the to remove waiting room

  // Entity addition/insertion
  // add all entities in the to add waiting room to the main entity vector
  m_entities.insert(m_entities.end(), m_entities_to_add.begin(),
                    m_entities_to_add.end());

  // // for each of the new entities, add them to the archetype manager
  // for (auto &freshEntityIndex : m_entitiesToAdd) {
  //   m_archetypeManager.assignArchetype(freshEntityIndex, {});
  // }

  m_entities_to_add.clear(); // clear the to add waiting room
}

////////////////////////////////////////////////////////////
const steamrot::ComponentFlags &
EntityManager::GetComponentFlags(size_t entity_id) {

  CMeta &meta_data =
      std::get<std::vector<CMeta>>(*(m_pool->getData()))[entity_id];

  return meta_data.m_component_flags;
}
