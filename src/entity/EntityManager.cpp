#include "EntityManager.h"


EntityManager::EntityManager(int poolSize) : m_pool(new EntityMemoryPool(poolSize))
{
}

size_t EntityManager::addEntity()
{
	size_t newEntityID = (*m_pool).getNextEntityIndex(); //get the next free index in the memory pool, set to active and return the index
	refreshEntity(*(*m_pool).getData(), newEntityID); //call the refresh entity function on the new ID to clear it
	std::get<std::vector<CMeta>>(*(*m_pool).getData())[newEntityID].activate(); //set the active vector to true at the new entity index
	m_entitiesToAdd.push_back(newEntityID);
	return newEntityID; //return the index of the new entity
}

void EntityManager::removeEntity(size_t entityID)
{
	m_entitiesToRemove.push_back(entityID); //add the id to the list of entities to remove
}

void EntityManager::updateWaitingRooms()
{
	for (const size_t& entityIndex : m_entitiesToRemove) //for all items in the to add list ...
	{
		std::get<std::vector<CMeta>>(*(*m_pool).getData())[entityIndex].deactivate(); //set the active vector to false at the passed entity index
		auto toRemove = std::find(m_entities.begin(), m_entities.end(), entityIndex);
		m_entities.erase(deadEntity); //remove the entity from the current active entities list
	}
	m_entitiesToRemove.clear(); //clear the to remove waiting room

	m_entities.insert(m_entities.end(), m_entitiesToAdd.begin(), m_entitiesToAdd.end()); //add the entities to add to the current entity list
	m_entitiesToAdd.clear(); //clear the to add waiting room
}

std::vector<size_t> EntityManager::getEntities()
{
	return m_entities;
}