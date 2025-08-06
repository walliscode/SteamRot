/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the EntityManager class
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "EntityManager.h"
#include "emp_helpers.h"

namespace steamrot {

////////////////////////////////////////////////////////////
EntityManager::EntityManager(const size_t &pool_size) {

  ResizeEntityMemoryPool(pool_size);
};

EntityMemoryPool &EntityManager::GetEntityMemoryPool() {
  // return a reference to the memory pool
  return m_entity_memory_pool;
}

const ArchetypeManager &EntityManager::GetArchetypeManager() {
  // return a const reference to the archetype manager
  return m_archetype_manager;
}

////////////////////////////////////////////////////////////
void EntityManager::ResizeEntityMemoryPool(const size_t pool_size) {

  // use std::apply to resize the memory pool with lambda function
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      m_entity_memory_pool);
}

////////////////////////////////////////////////////////////
size_t EntityManager::GetNextFreeEntityIndex() {

  // get CMeta vector from the pool
  std::vector<CMeta> meta_data =
      emp_helpers::GetComponentVector<CMeta>(m_entity_memory_pool);

  // find next inactive entity index
  for (size_t i = 0; i < meta_data.size(); ++i) {
    if (!meta_data[i].m_entity_active) {
      return i;
    }
  }
  // if no inactive entity found, return the size of the vector
  return meta_data.size();
};

} // namespace steamrot
