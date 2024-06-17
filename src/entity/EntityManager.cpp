#include "EntityManager.h"
#include "component_generated.h"
#include "general_util.h"
#include <fstream>



EntityManager::EntityManager(std::string sceneName, size_t poolSize) : m_pool(new EntityMemoryPool(poolSize))
{
	std::cout << "Initialising entities" << std::endl;
	intialiseEntities(sceneName);
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
		m_entities.erase(toRemove); //remove the entity from the current active entities list
	}
	m_entitiesToRemove.clear(); //clear the to remove waiting room

	m_entities.insert(m_entities.end(), m_entitiesToAdd.begin(), m_entitiesToAdd.end()); //add the entities to add to the current entity list
	m_entitiesToAdd.clear(); //clear the to add waiting room
}

void EntityManager::intialiseEntities(std::string sceneName)
{
	
	std::string fileName = (std::string(FB_BINARIES_PATH) + sceneName + ".bin");
	// check binary file exists

	std::cout << "Reading binary file: " << fileName << std::endl;

	std::vector<std::byte> buffer = utils::readBinaryFile(fileName);

	//if buffer is empty exit intialiseEntities
	if (buffer.empty()) {
		std::cout << "No Entities to intialise" << std::endl;
		return;
	}

	const SteamRot::rawData::EntityList* entityList = SteamRot::rawData::GetEntityList(buffer.data());

	for (const auto entity : *entityList->entities())
	{
		size_t entityID = addEntity();
		if (entity->transform()) {
			auto transform = entity->transform();
			auto& cTransform = getComponent<CTransform>(entityID); //get the transform component at the new entity index
			cTransform.setHas(true); //set the has value to true (has a transform component
			cTransform.position.x = transform->position()->x();
			cTransform.position.y = transform->position()->y();
			cTransform.velocity.x = transform->velocity()->x();
			cTransform.velocity.y = transform->velocity()->y();
			
		}
	}

	updateWaitingRooms(); // update the active entities list, addEntity() adds the entiy to m_entitiesToAdd, updateWaitingRooms() adds the entities to m_entities


}

std::vector<size_t> EntityManager::getEntities()
{
	return m_entities;
}