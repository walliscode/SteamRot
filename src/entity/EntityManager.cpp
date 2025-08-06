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

  // std::cout << "Creating EntityManager with pool size: " << pool_size
  //           << std::endl;
  // // create the memory pool with the given size
  // m_pool =
  //     std::make_unique<steamrot::components::containers::EntityMemoryPool>();
  // ResizePool(pool_size);
  //
  // std::cout << "Attempting to configure entities from default data..."
  //           << std::endl;
  // // configure the entities in the memory pool only if entities_data is not
  // null
  // if (entity_collection != nullptr) {
  //   m_entity_configuration_factory.ConfigureEntitiesFromDefaultData(
  //       *m_pool, entity_collection);
  //   std::cout << "Entities configured from default data." << std::endl;
  // }
  //
  // std::cout << "Generating archetypes..." << std::endl;
  // // map out current archetypes
  // m_archetype_manager.GenerateAllArchetypes(*m_pool);
};

components::containers::EntityMemoryPool &EntityManager::GetEntityMemoryPool() {
  // return a reference to the memory pool
  return m_pool;
}

const ArchetypeManager &EntityManager::GetArchetypeManager() {
  // return a const reference to the archetype manager
  return m_archetype_manager;
}

////////////////////////////////////////////////////////////
void EntityManager::ResizePool(const size_t &pool_size) {

  // use std::apply to resize the memory pool with lambda function
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      m_pool);
}

////////////////////////////////////////////////////////////
size_t EntityManager::GetNextFreeEntityIndex() {

  // get CMeta vector from the pool
  std::vector<CMeta> meta_data = emp_helpers::GetComponentVector<CMeta>(m_pool);

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
