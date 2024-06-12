#include "Archetype.h"

Archetype::Archetype(unsigned int archetypeID) : m_id(archetypeID) {}

const unsigned int Archetype::getID() const {
	return m_id;
}

const std::vector<int>& Archetype::getEntityIDs() const {
	return m_entityIDs;
}