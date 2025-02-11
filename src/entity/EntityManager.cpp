#include "EntityManager.h"
#include "general_util.h"
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

EntityManager::EntityManager(const size_t &poolSize) {

  m_pool = std::make_shared<EntityMemoryPool>(poolSize);
}

// find next inactive entity index
// refresh all components at that index
// turn "on" by switching CMeta.active to true
size_t EntityManager::AddEntity() {

  size_t new_entity_id = (*m_pool).getNextEntityIndex();

  RefreshEntity(*(*m_pool).getData(), new_entity_id);

  std::vector<CMeta> meta_data =
      std::get<std::vector<CMeta>>(*(m_pool->getData()));

  meta_data[new_entity_id].activate();
  m_entities_to_add.push_back(new_entity_id);

  return new_entity_id;
}

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

// preload entities from json file, essentially adding Component data at each
// index
void EntityManager::IntialiseEntities(std::string scene_name) {

  std::string file_name =
      (std::string(RESOURCES_DIR) + "/jsons/entities_" + scene_name + ".json");
  // check binary file exists

  bool file_exists = utils::CheckFileExists(file_name);

  if (!file_exists) {
    return;
  };

  // load entity configuration
  std::ifstream f(file_name);
  json entity_config = json::parse(f);

  // add a template function to load config from json
}

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
