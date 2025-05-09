#include "ArchetypeManager.h"
#include <cstddef>
#include <magic_enum/magic_enum.hpp>
#include <vector>

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
