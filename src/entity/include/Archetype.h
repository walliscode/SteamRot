#pragma once

#include <vector>

class Archetype {
private:
    std::vector<int> m_entityIDs;
    unsigned int m_id;

public:
    Archetype(unsigned int archetypeID);

    const unsigned int getID() const;
    const std::vector<int>& getEntityIDs() const;

};
