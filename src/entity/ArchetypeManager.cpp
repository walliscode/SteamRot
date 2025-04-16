#include "ArchetypeManager.h"
#include <cstddef>
#include <magic_enum/magic_enum.hpp>
#include <stdexcept>
#include <vector>

////////////////////////////////////////////////////////////
ArchetypeManager::ArchetypeManager() {}

////////////////////////////////////////////////////////////
const Archetype &ArchetypeManager::GetExactArchetype(
    const SteamRot::ComponentFlags &archetype_finder) const {

  for (auto &arch : m_archetypes) {
    if (arch.GetID() == archetype_finder) {
      return arch;
    }
  }

  throw std::runtime_error("Archetype not found");
};

////////////////////////////////////////////////////////////
const std::vector<size_t> &ArchetypeManager::GetExactArchetypeEntities(
    const SteamRot::ComponentFlags &archetype_finder) const {

  return GetExactArchetype(archetype_finder).GetEntities();
};

////////////////////////////////////////////////////////////
const std::shared_ptr<std::vector<Archetype>>
ArchetypeManager::GetInclusiveArchetype(
    const SteamRot::ComponentFlags &archetype_requirements) const {
  std::vector<Archetype> return_set;

  // Loop through all archetypes adding each code that contains at least the tag
  // code

  for (auto &arch : m_archetypes) {
    // use & bit wise operator from magic enum to get inclusive archetypes
    // this will return a non-zero value if the archetype contains the required
    // components
    using namespace magic_enum::bitwise_operators;
    int flag_check = static_cast<int>(arch.GetID() & archetype_requirements);
    if (flag_check != 0) {
      return_set.push_back(arch);
    }
  }
  return std::make_shared<std::vector<Archetype>>(return_set);
}

////////////////////////////////////////////////////////////
const std::shared_ptr<std::vector<size_t>>
ArchetypeManager::GetInclusiveArchetypeEntities(
    const SteamRot::ComponentFlags &archetype_requirements) const {

  // create a vector to store all entities for all matching archetypes' entities
  std::vector<size_t> entities_set;
  std::vector<Archetype> archetype_set =
      *GetInclusiveArchetype(archetype_requirements);

  // loop through all archetypes and add all entities to the set
  // this currently does not care about duplicates
  for (auto &arch : archetype_set) {
    entities_set.insert(entities_set.end(), arch.GetEntities().begin(),
                        arch.GetEntities().end());
  }
  return std::make_shared<std::vector<size_t>>(entities_set);
}
