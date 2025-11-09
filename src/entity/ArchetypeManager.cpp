/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ArchetypeManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ArchetypeManager.h"
#include "containers.h"
#include "entity_memory.h"
#include <cstddef>
#include <expected>
#include <magic_enum/magic_enum.hpp>
#include <variant>

namespace steamrot {
////////////////////////////////////////////////////////////
ArchetypeManager::ArchetypeManager(const EntityMemoryPool &emp)
    : m_entity_memory_pool(emp) {}

/////////////////////////////////////////////////
std::expected<const ArchetypeID, FailInfo>
ArchetypeManager::GenerateArchetypeID(size_t entity_index) {

  // create blank archetypeID
  ArchetypeID archetypeID{0};

  // iterate over all the vectors in the entity memory pool
  std::apply(
      [&](const auto &...component_vector) {
        // for each component vector, check if the entity has that component

        ((archetypeID.set(
             component_vector[entity_index].GetComponentRegisterIndex(),
             component_vector[entity_index].m_active)),
         ...);
      },
      m_entity_memory_pool);

  return archetypeID;
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ArchetypeManager::GenerateAllArchetypes() {
  // clear existing archetypes
  m_archetypes.clear();

  // get current pool size
  size_t pool_size = entity::memory::GetMemoryPoolSize(m_entity_memory_pool);

  // iterate over all entities in the memory pool
  for (size_t entity_index = 0; entity_index < pool_size; ++entity_index) {

    // check if attempt to generate archetype id is successful
    auto id_result = GenerateArchetypeID(entity_index);
    if (!id_result.has_value()) {
      return std::unexpected(
          id_result.error()); // return error if failed to generate ID
    }
    // get the component mask for the current entity
    ArchetypeID archetypeID = GenerateArchetypeID(entity_index).value();

    // [] operator should create a new vector if the key does not exist, hence
    // this over .find()
    m_archetypes[archetypeID].push_back(entity_index);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
const std::unordered_map<ArchetypeID, Archetype> &
ArchetypeManager::GetArchetypes() const {
  return m_archetypes;
}
} // namespace steamrot
