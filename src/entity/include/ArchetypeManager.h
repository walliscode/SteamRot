#pragma once

#include <vector>
#include <cstdlib>
#include "Archetype.h"

class ArchetypeManager {
private:
    std::vector<Archetype> m_archetypes;

public:
    ArchetypeManager();

    const Archetype& getArchetype() const; //get the archetype for the component set passed
    const std::vector<size_t>& getArchetypeEntities() const; //get the set of entity ids for the component set passed
    void assignArchetype(size_t assEntity); //assign a new entity ID to the correct archtype
    void clearEntity(size_t clrEntity); //remove an entity ID from the relevant archetype
    void reassessEntity(size_t reAssEntity); //re-assess an entity an assign to correct archetype based on current components

};