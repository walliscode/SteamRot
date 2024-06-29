#pragma once
#include "Archetype.h"
#include <algorithm>
#include <memory>

Archetype::Archetype(std::vector<size_t> idSet) : m_idSet(idSet) {}

const std::vector<size_t>& Archetype::getIDSet() const {
	return m_idSet; //get the ID set for this archetype (may be redundant with the code generation)
}

const std::vector<size_t>& Archetype::getEntities() const {
	return m_entities; //return the list of IDs
}

void Archetype::addEntity(size_t newEntity) {
	m_entities.push_back(newEntity); //add the new identity ID to the list
}

void Archetype::removeEntity(size_t remEntity) {
	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), remEntity), m_entities.end()); //remove the target entity ID
}

const size_t& Archetype::getCode() const {
	size_t archCode = 0;
	for (auto& compID : m_idSet) {
		archCode = archCode | (1 << (compID - 1)); //for each ID, OR the current code with 1, bitshifted by the ID - 1 (add a 1 to the code at the binary position set by the ID)
	};
	return archCode;
}