#include "EntityManager.h"
#include "general_util.h"
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

EntityManager::EntityManager(const size_t &pool_size,
                             const std::string &scene_name) {

  m_pool = std::make_shared<EntityMemoryPool>(pool_size);

  // add all entites in json file
  IntialiseEntities(scene_name);
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

void EntityManager::RemoveEntity(size_t entity_index) {

  m_entities_to_remove.push_back(entity_index);
}

void EntityManager::UpdateWaitingRooms() {
  // Entity removal/deleteion
  // for each entity to remove, deactivate the CMeta component
  for (const size_t &entity_index : m_entities_to_remove) {

    std::get<std::vector<CMeta>>(*(m_pool->getData()))[entity_index]
        .deactivate();

    // get iterator to entity to remove
    auto to_remove =
        std::find(m_entities.begin(), m_entities.end(), entity_index);

    // m_archetypeManager.clearEntity(
    //     entityIndex, {});

    // remove from vector of current entitie indices
    m_entities.erase(to_remove);
  }
  m_entities_to_remove.clear(); // clear the to remove waiting room

  // Entity addition/insertion
  // add all entities in the to add waiting room to the main entity vector
  m_entities.insert(m_entities.end(), m_entities_to_add.begin(),
                    m_entities_to_add.end());

  // // for each of the new entities, add them to the archetype manager
  // for (auto &freshEntityIndex : m_entitiesToAdd) {
  //   m_archetypeManager.assignArchetype(freshEntityIndex, {});
  // }

  m_entities_to_add.clear(); // clear the to add waiting room
}

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

  // the components will need to be added one by one here
  // if we find we are initiliasing them else from string then pull out into a
  // function

  for (auto entity : entity_config) {

    // CMeta activation handled by AddEntity
    size_t entity_id = AddEntity();
  }
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
