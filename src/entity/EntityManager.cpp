#include "EntityManager.h"

EntityManager::EntityManager(const size_t &poolSize) {

  m_pool = std::make_shared<EntityMemoryPool>(poolSize);
}
//
// size_t EntityManager::addEntity() {
//   size_t newEntityID =
//       (*m_pool)
//           .getNextEntityIndex(); // get the next free index in the memory
//           pool,
//                                  // set to active and return the index
//   refreshEntity(*(*m_pool).getData(),
//                 newEntityID); // call the refresh entity function on the new
//                 ID
//                               // to clear it
//   std::get<std::vector<CMeta>>(*(*m_pool).getData())[newEntityID]
//       .activate(); // set the active vector to true at the new entity index
//   m_entitiesToAdd.push_back(newEntityID);
//
//   return newEntityID; // return the index of the new entity
// }

// void EntityManager::removeEntity(size_t entityID) {
//   m_entitiesToRemove.push_back(
//       entityID); // add the id to the list of entities to remove
// }

// void EntityManager::updateWaitingRooms() {
//   for (const size_t &entityIndex :
//        m_entitiesToRemove) // for all items in the to add list ...
//   {
//     std::get<std::vector<CMeta>>(*(*m_pool).getData())[entityIndex]
//         .deactivate(); // set the active vector to false at the passed entity
//                        // index
//     auto toRemove =
//         std::find(m_entities.begin(), m_entities.end(), entityIndex);
//
//     m_archetypeManager.clearEntity(
//         entityIndex, {}); // remove the entity from the archetype manager
//     m_entities.erase(
//         toRemove); // remove the entity from the current active entities list
//   }
//   m_entitiesToRemove.clear(); // clear the to remove waiting room
//
//   m_entities.insert(
//       m_entities.end(), m_entitiesToAdd.begin(),
//       m_entitiesToAdd
//           .end()); // add the entities to add to the current entity list
//   // for each of the new entities, add them to the archetype manager
//   for (auto &freshEntityIndex : m_entitiesToAdd) {
//     m_archetypeManager.assignArchetype(freshEntityIndex, {});
//   }
//
//   m_entitiesToAdd.clear(); // clear the to add waiting room
// }

// load "entities" from a json file
// an entity is actually a collection of components at an index
// void EntityManager::intialiseEntities(std::string sceneName) {
//
//   std::string fileName =
//       (std::string(RESOURCES_DIR) + "/jsons/entities_" + sceneName +
//       ".json");
//   // check binary file exists
//
//   std::cout << "Reading binary file: " << fileName << std::endl;
//
//   std::vector<std::byte> buffer = utils::readBinaryFile(fileName);
//
//   // if buffer is empty exit intialiseEntities
//   if (buffer.empty()) {
//     std::cout << "No Entities to intialise" << std::endl;
//     return;
//   }
// }

// std::vector<size_t> EntityManager::getEntities() { return m_entities; }

// convert the component data to json and other entity data
json EntityManager::toJSON() {
  json j;

  // // add data for each entity under "entityData" key
  // for (auto &entity : m_entities) {
  //   json entityData;
  //   entityData["entityID"] = entity;
  //   entityData["CTransform"] = getComponent<CTransform>(entity).toJSON();
  //   entityData["CText"] = getComponent<CText>(entity).toJSON();
  //   entityData["CMeta"] = getComponent<CMeta>(entity).toJSON();
  //   j["entityData"].push_back(entityData);
  // }

  return j;
}
//
// const ArchetypeManager &EntityManager::getArchetypeManager() const {
//   return m_archetypeManager;
// }
//
// // Pass through functions for handling archetypes
//
// const Archetype &
// EntityManager::getExactArchetype(std::vector<std::string> requirements) const
// {
//   return m_archetypeManager.getExactArchetype(requirements);
// }
// const std::vector<size_t> &EntityManager::getExactArchetypeEntities(
//     std::vector<std::string> requirements) const {
//   return m_archetypeManager.getExactArchetypeEntities(requirements);
// }
//
// const std::shared_ptr<std::vector<Archetype>>
// EntityManager::getInclusiveArchetype(
//     std::vector<std::string> requirements) const {
//   return m_archetypeManager.getInclusiveArchetype(requirements);
// }
// const std::shared_ptr<std::vector<size_t>>
// EntityManager::getInclusiveArchetypeEntities(
//     std::vector<std::string> requirements) const {
//   return m_archetypeManager.getInclusiveArchetypeEntities(requirements);
// }
