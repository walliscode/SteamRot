/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the EntityManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityManager.h"
#include "FailInfo.h"
#include "containers.h"
#include "entity_memory.h"
#include <expected>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
EntityManager::EntityManager() : m_archetype_manager(m_entity_memory_pool) {}

/////////////////////////////////////////////////
EntityMemoryPool &EntityManager::GetEntityMemoryPool() {
  // return a reference to the memory pool
  return m_entity_memory_pool;
}

/////////////////////////////////////////////////
const EntityMemoryPool &EntityManager::GetEntityMemoryPool() const {
  // return a const reference to the memory pool
  return m_entity_memory_pool;
}

/////////////////////////////////////////////////
const ArchetypeManager &EntityManager::GetArchetypeManager() const {
  // return a const reference to the archetype manager
  return m_archetype_manager;
}

/////////////////////////////////////////////////
size_t EntityManager::GetNextFreeEntityIndex() {

  // get CMeta vector from the pool
  std::vector<CMeta> meta_data =
      entity::memory::GetComponentVector<CMeta>(m_entity_memory_pool);

  // find next inactive entity index
  for (size_t i = 0; i < meta_data.size(); ++i) {
    if (!meta_data[i].m_entity_active) {
      return i;
    }
  }
  // if no inactive entity found, return the size of the vector
  return meta_data.size();
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> EntityManager::GenerateAllArchetypes() {
  auto generate_result = m_archetype_manager.GenerateAllArchetypes();
  if (!generate_result.has_value()) {
    return std::unexpected(generate_result.error());
  }
  return std::monostate{};
}

} // namespace steamrot
