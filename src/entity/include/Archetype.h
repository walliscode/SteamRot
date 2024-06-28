#pragma once

#include <vector>

class Archetype {
private:
    std::vector<size_t> m_entities;
    std::vector<size_t> m_idSet;

public:
    Archetype(std::vector<size_t> archetypeID);

    const std::vector<size_t>& getIDSet() const; //get the archetype key set
    const std::vector<size_t>& getEntities() const; //get the set of entity ids in this archetype
    void addEntity(size_t newEntity); //add an entity ID to this archetype
    void removeEntity(size_t remEntity); //remove an entity ID from this archetype

    const size_t& getCode() const;
};