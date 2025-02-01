#include "EntityManager.h"
#include "GameEngine.h"
#include "Scene.h"
#include "general_util.h"

EntityManager::EntityManager(size_t poolSize, Scene &scene)
    : m_pool(new EntityMemoryPool(poolSize)), m_scene(scene) {}

size_t EntityManager::addEntity() {
  size_t newEntityID =
      (*m_pool)
          .getNextEntityIndex(); // get the next free index in the memory pool,
                                 // set to active and return the index
  refreshEntity(*(*m_pool).getData(),
                newEntityID); // call the refresh entity function on the new ID
                              // to clear it
  std::get<std::vector<CMeta>>(*(*m_pool).getData())[newEntityID]
      .activate(); // set the active vector to true at the new entity index
  m_entitiesToAdd.push_back(newEntityID);

  return newEntityID; // return the index of the new entity
}

void EntityManager::removeEntity(size_t entityID) {
  m_entitiesToRemove.push_back(
      entityID); // add the id to the list of entities to remove
}

void EntityManager::updateWaitingRooms() {
  for (const size_t &entityIndex :
       m_entitiesToRemove) // for all items in the to add list ...
  {
    std::get<std::vector<CMeta>>(*(*m_pool).getData())[entityIndex]
        .deactivate(); // set the active vector to false at the passed entity
                       // index
    auto toRemove =
        std::find(m_entities.begin(), m_entities.end(), entityIndex);

    m_archetypeManager.clearEntity(
        entityIndex, {}); // remove the entity from the archetype manager
    m_entities.erase(
        toRemove); // remove the entity from the current active entities list
  }
  m_entitiesToRemove.clear(); // clear the to remove waiting room

  m_entities.insert(
      m_entities.end(), m_entitiesToAdd.begin(),
      m_entitiesToAdd
          .end()); // add the entities to add to the current entity list
  // for each of the new entities, add them to the archetype manager
  for (auto &freshEntityIndex : m_entitiesToAdd) {
    m_archetypeManager.assignArchetype(freshEntityIndex, {});
  }

  m_entitiesToAdd.clear(); // clear the to add waiting room
}

void EntityManager::intialiseEntities(std::string sceneName) {

  std::string fileName =
      (std::string(FB_BINARIES_PATH) + sceneName + "_entities" + ".bin");
  // check binary file exists

  std::cout << "Reading binary file: " << fileName << std::endl;

  std::vector<std::byte> buffer = utils::readBinaryFile(fileName);

  // if buffer is empty exit intialiseEntities
  if (buffer.empty()) {
    std::cout << "No Entities to intialise" << std::endl;
    return;
  }

  std::cout << "Getting flatbuffer entity list" << std::endl;
  const SteamRot::rawData::EntityList *entityList =
      SteamRot::rawData::GetEntityList(buffer.data());

  std::cout << "Iterating through entity list" << std::endl;
  for (const auto entity : *entityList->entities()) {
    // create a new entity (this will create all the new components
    size_t entityID = addEntity();

    // if the flatbuffers buffer has the data then add to that entity
    if (entity->transform()) {
      auto &cTransform = getComponent<CTransform>(
          entityID); // get the transform component at the new entity index
      cTransform.fromFlatbuffers(
          entity->transform()); // populate the transform component with the
                                // flatbuffer transform data
      std::cout << "Transform added for entity: " << entityID << std::endl;
    }

    if (entity->text_display()) {

      auto &cText = getComponent<CText>(
          entityID); // get the text component at the new entity index
      const auto &font = m_scene.getSceneManager().getAssetManager().getFont(
          entity->text_display()
              ->font()
              ->str()); // get the font from the asset manager
      cText.fromFlatbuffers(
          entity->text_display(),
          font); // populate the text component with the flatbuffer text data
      std::cout << "Text added for entity: " << entityID << std::endl;
    }

    if (entity->meta()) {
      auto &cMeta = getComponent<CMeta>(
          entityID); // get the meta component at the new entity index
      cMeta.fromFlatbuffers(entity->meta()); // populate the meta component with
                                             // the flatbuffer meta data
      std::cout << "Meta added for entity: " << entityID << std::endl;
    }
  }
  std::cout << "Entities intialised" << std::endl;

  std::cout << "Updating waiting rooms" << std::endl;
  updateWaitingRooms(); // update the active entities list, addEntity() adds the
                        // entiy to m_entitiesToAdd, updateWaitingRooms() adds
                        // the entities to m_entities
}

EntityMemoryPool &EntityManager::getPool() { return *m_pool; }

std::vector<size_t> EntityManager::getEntities() { return m_entities; }

// convert the component data to json and other entity data
json EntityManager::toJSON() {
  json j;

  // add data for each entity under "entityData" key
  for (auto &entity : m_entities) {
    json entityData;
    entityData["entityID"] = entity;
    entityData["CTransform"] = getComponent<CTransform>(entity).toJSON();
    entityData["CText"] = getComponent<CText>(entity).toJSON();
    entityData["CMeta"] = getComponent<CMeta>(entity).toJSON();
    j["entityData"].push_back(entityData);
  }

  return j;
}

const ArchetypeManager &EntityManager::getArchetypeManager() const {
  return m_archetypeManager;
}

// Pass through functions for handling archetypes

const Archetype &
EntityManager::getExactArchetype(std::vector<std::string> requirements) const {
  return m_archetypeManager.getExactArchetype(requirements);
}
const std::vector<size_t> &EntityManager::getExactArchetypeEntities(
    std::vector<std::string> requirements) const {
  return m_archetypeManager.getExactArchetypeEntities(requirements);
}

const std::shared_ptr<std::vector<Archetype>>
EntityManager::getInclusiveArchetype(
    std::vector<std::string> requirements) const {
  return m_archetypeManager.getInclusiveArchetype(requirements);
}
const std::shared_ptr<std::vector<size_t>>
EntityManager::getInclusiveArchetypeEntities(
    std::vector<std::string> requirements) const {
  return m_archetypeManager.getInclusiveArchetypeEntities(requirements);
}
