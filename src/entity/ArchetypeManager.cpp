#include "ArchetypeManager.h"
#include "Config.h"
#include <iostream>

ArchetypeManager::ArchetypeManager() {}

const Archetype& ArchetypeManager::getExactArchetype(const std::vector<std::string> requirements) const {
	
	//Gen the target archetype code
	std::unique_ptr<size_t> tagCode =  generateTagCode(requirements);

	//Loop through all archetypes until the exact matching code is found

	for (auto& arch : m_archetypes) {
		if (*tagCode == *arch.getCode()) {
			//return the matching archetype
			return arch;
			break;
		}
	}
}

const std::shared_ptr<std::vector<Archetype>> ArchetypeManager::getInclusiveArchetype(const std::vector<std::string> requirements) const {
	std::vector<Archetype> returnSet;

	//Gen the target archetype code
	std::unique_ptr<size_t> tagCode = generateTagCode(requirements);

	//Loop through all archetypes adding each code that contains at least the tag code

	for (auto& arch : m_archetypes) {
		size_t tagAnd = *tagCode & *arch.getCode();
		if (tagAnd == *tagCode) {
			returnSet.push_back(arch);
		}
	}
	return std::make_shared <std::vector<Archetype>>(returnSet);
}



const std::vector<size_t>& ArchetypeManager::getExactArchetypeEntities(const std::vector<std::string> requirements) const {
	return getExactArchetype(requirements).getEntities(); //return the entities for the given component set
}

const std::shared_ptr<std::vector<size_t>> ArchetypeManager::getInclusiveArchetypeEntities(const std::vector<std::string> requirements) const {
	
	//create a vector to store all entities for all matching archetypes' entities
	std::vector<size_t>  entitiesSet;
	std::vector<Archetype> archSet = *getInclusiveArchetype(requirements);
	for (auto& arch : archSet) {
		entitiesSet.insert(entitiesSet.end(), arch.getEntities().begin(), arch.getEntities().end());
	}
	return std::make_shared <std::vector<size_t>>(entitiesSet); //return the entities for the given component set
}


const std::unique_ptr<size_t> ArchetypeManager::generateTagCode(std::vector<std::string> tags) const{
	size_t archCode = 0;
	for (auto& tag : tags) {
		archCode = archCode | (1 << (compTagMap[tag] - 1)); //for each ID, OR the current code with 1, bitshifted by the ID - 1 (add a 1 to the code at the binary position set by the ID)
	};
	return std::make_unique<size_t>(archCode);
}


void ArchetypeManager::assignArchetype(size_t assEntity, std::vector<std::string> compTags) {
	//Gen the archcode for the given entity
	std::unique_ptr<size_t> entCode = generateTagCode(compTags);
	//loop through the arch lists and add this entity to the correct list

	for (auto& arch : m_archetypes) {
		if (*arch.getCode() == *entCode) {
			arch.addEntity(assEntity);
			return;
		}
	}

	//if no archetype existed with the correct code, create a new archetype
	std::vector<size_t> newIDSet;
	for (auto& tag : compTags) {
		newIDSet.push_back(compTagMap[tag]);
	}
	m_archetypes.push_back(*new Archetype(newIDSet));
	m_archetypes.back().addEntity(assEntity);

}

void ArchetypeManager::clearEntity(size_t clrEntity, std::vector<std::string> compTags) {
	//Gen the archcode for the given entity
	std::unique_ptr<size_t> entCode = generateTagCode(compTags);

	//loop through the arch lists and remove this entity from the correct list
	for (auto& arch : m_archetypes) {
		if (*arch.getCode() == *entCode) {
			arch.removeEntity(clrEntity);
			break;
		}
	}
}

void ArchetypeManager::reassessEntity(size_t reAssEntity, std::vector<std::string> compTags) {
	//find the old archetype and remove this entity
	for (auto& arch : m_archetypes) {
		if (arch.contains(reAssEntity)) {
			arch.removeEntity(reAssEntity);
			break;
		}
	}

	//add the entity to the correct list
	assignArchetype(reAssEntity, compTags);
}

const std::vector<Archetype> ArchetypeManager::getArchetypes() const{
	return m_archetypes;
}