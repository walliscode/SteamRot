////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "EntityManager.h"
#include <memory>

using json = nlohmann::json;

namespace steamrot {

////////////////////////////////////////////////////////////
EntityManager::EntityManager(const size_t &pool_size)
    : m_entity_configuration_factory() {

  // create the memory pool with the given size
  m_pool =
      std::make_unique<steamrot::components::containers::EntityMemoryPool>();
  ResizePool(pool_size);
};

////////////////////////////////////////////////////////////
void EntityManager::ResizePool(const size_t &pool_size) {

  // use std::apply to resize the memory pool with lambda function
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      *m_pool);
}

////////////////////////////////////////////////////////////
size_t EntityManager::GetNextFreeEntityIndex() {

  // get CMeta vector from the pool
  std::vector<CMeta> meta_data = GetComponentVector<CMeta>();

  // find next inactive entity index
  for (size_t i = 0; i < meta_data.size(); ++i) {
    if (!meta_data[i].m_entity_active) {
      return i;
    }
  }
  // if no inactive entity found, return the size of the vector
  return meta_data.size();
};

////////////////////////////////////////////////////////////
size_t EntityManager::AddEntity() {

  // find next inactive entity index
  size_t new_entity_id = GetNextFreeEntityIndex();

  // reset all components for the new entity
  RefreshEntity(*m_pool, new_entity_id);

  // get CMeta vector from the pool
  std::vector<CMeta> &meta_data = GetComponentVector<CMeta>();

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

    std::get<std::vector<CMeta>>(*m_pool)[entity_index].m_entity_active = false;

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
} // namespace steamrot
