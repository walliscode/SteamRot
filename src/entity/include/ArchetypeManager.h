#pragma once

#include <vector>
#include <string>
#include <cstdlib>
#include "Archetype.h"

class ArchetypeManager {
private:
    std::vector<Archetype> m_archetypes;

public:
    ArchetypeManager();

    const Archetype& getExactArchetype(std::vector<std::string> requirements) const; //get the archetype for the component set including all AND ONLY all requirements
    const std::vector<size_t>& getExactArchetypeEntities(std::vector<std::string> requirements) const; //get the set of entity ids for the requested archeype

    const std::shared_ptr<std::vector<Archetype>> getInclusiveArchetype(std::vector<std::string> requirements) const; //get the archetype for any componet set including all requirements
    const std::shared_ptr <std::vector<size_t>> getInclusiveArchetypeEntities(std::vector<std::string> requirements) const; //get the set of entity ids for the requested archeype
    
    const std::unique_ptr<size_t> generateTagCode(std::vector<std::string> tags) const; //generate the archetype code for the given requirement tags

    void assignArchetype(size_t assEntity, std::vector<std::string>  compTags); //assign a new entity ID to the correct archtype
    void clearEntity(size_t clrEntity, std::vector<std::string>  compTags); //remove an entity ID from the relevant archetype
    void reassessEntity(size_t reAssEntity, std::vector<std::string>  compTags); //re-assess an entity an assign to correct archetype based on current components

    const std::vector<Archetype> getArchetypes() const; //return all the archetpes from the manager
};