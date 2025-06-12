#include "ArchetypeManager.h"
#include "EntityHelpers.h"
#include <cstddef>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <vector>

namespace steamrot {
////////////////////////////////////////////////////////////
ArchetypeManager::ArchetypeManager() {}

////////////////////////////////////////////////////////////
std::vector<size_t> ArchetypeManager::GetEntityIndexes(
    const std::vector<ArchetypeID> &archetype_IDs) const {

  // create return vector
  std::vector<size_t> entityIndexes;

  // iterate over provided archetypeIDs
  for (const auto &archetypeID : archetype_IDs) {

    // find the archetype in the map
    auto it = m_archetypes.find(archetypeID);

    // if found, add the entity indexes to the return vector
    if (it != m_archetypes.end()) {
      const Archetype &archetype = it->second;
      entityIndexes.insert(entityIndexes.end(), archetype.begin(),
                           archetype.end());

    } else {
      // no error needed, it just means that ArcchtypeID is currently empty
    }
  }

  return entityIndexes;
}
const ArchetypeID ArchetypeManager::GenerateArchetypeID(
    const components::containers::EntityMemoryPool &entity_memory_pool,
    size_t entityIndex) {

  // create blank archetypeID
  ArchetypeID archetypeID{0};

  // iterate over all the vectors in the entity memory pool
  std::apply(
      [&](const auto &...component_vector) {
        // for each component vector, check if the entity has that component

        ((archetypeID.set(
             component_vector[entityIndex].GetComponentRegisterIndex(),
             component_vector[entityIndex].m_active)),
         ...);
      },
      entity_memory_pool);

  return archetypeID;
};

void ArchetypeManager::GenerateAllArchetypes(
    const components::containers::EntityMemoryPool &entity_memory_pool) {
  // clear existing archetypes
  m_archetypes.clear();

  // get current pool size
  size_t pool_size = GetMemoryPoolSize(entity_memory_pool);
  std::cout << "Generating archetypes for pool size: " << pool_size
            << std::endl;
  // iterate over all entities in the memory pool
  for (size_t entityIndex = 0; entityIndex < pool_size; ++entityIndex) {

    // get the component mask for the current entity
    ArchetypeID archetypeID =
        GenerateArchetypeID(entity_memory_pool, entityIndex);

    // if the archetype already exists, add the entity index to it
    if (m_archetypes.find(archetypeID) != m_archetypes.end()) {
      m_archetypes[archetypeID].push_back(entityIndex);
    } else {

      // otherwise, create a new archetype with this entity index
      m_archetypes[archetypeID] = {entityIndex};
    }
  }
}

std::unordered_map<ArchetypeID, Archetype> &ArchetypeManager::GetArchetypes() {
  return m_archetypes;
}
} // namespace steamrot
