////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "EntityManager.h"
#include "general_util.h"

using json = nlohmann::json;
using namespace magic_enum::bitwise_operators;

////////////////////////////////////////////////////////////
EntityManager::EntityManager(const size_t &pool_size,
                             const std::string &scene_name)
    : m_entity_configuration_factory(scene_name) {

  m_pool = std::make_shared<EntityMemoryPool>(pool_size);

  // add all entites in json file
  InitialiseEntities(scene_name);
}

////////////////////////////////////////////////////////////
size_t EntityManager::AddEntity() {

  size_t new_entity_id = (*m_pool).getNextEntityIndex();

  RefreshEntity(*(*m_pool).getData(), new_entity_id);

  std::vector<CMeta> meta_data =
      std::get<std::vector<CMeta>>(*(m_pool->getData()));

  // switch on the entity
  meta_data[new_entity_id].m_entity_active = true;
  m_entities_to_add.push_back(new_entity_id);

  return new_entity_id;
}

////////////////////////////////////////////////////////////
void EntityManager::RemoveEntity(size_t entity_index) {

  m_entities_to_remove.push_back(entity_index);
}

////////////////////////////////////////////////////////////
void EntityManager::UpdateWaitingRooms() {
  // Entity removal/deleteion
  // for each entity to remove, deactivate the CMeta component
  for (const size_t &entity_index : m_entities_to_remove) {

    std::get<std::vector<CMeta>>(*(m_pool->getData()))[entity_index]
        .m_entity_active = false;

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

////////////////////////////////////////////////////////////
void EntityManager::InitialiseEntities(std::string scene_name) {

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

    // pull out ComponentFlag for use
    SteamRot::ComponentFlags &component_flags =
        GetComponent<CMeta>(entity_id).m_component_flags;
    // each component will need to be added here manually
    // hopefully at some point i will figure out how to represent a string map
    // with types
    //
    // json.template get<> calls the from_json function from the component
    // vector component is the actual compnent in the pool
    // overwriting the default constructed component
    // component_flags is turned on for that component
    for (auto &component : entity) {
      if (component["type"] == "CMeta") {

        CMeta json_created_component = component.template get<CMeta>();
        CMeta &vector_component = GetComponent<CMeta>(entity_id);
        vector_component = json_created_component;

        component_flags = component_flags | SteamRot::ComponentFlags::CMeta;

      } else if (component["type"] == "CShape") {
        CShape json_created_component = component.template get<CShape>();
        CShape &vector_component = GetComponent<CShape>(entity_id);
        vector_component = json_created_component;

        component_flags = component_flags | SteamRot::ComponentFlags::CShape;
      }
    }
  }
}

////////////////////////////////////////////////////////////
const SteamRot::ComponentFlags &
EntityManager::GetComponentFlags(size_t entity_id) {

  CMeta &meta_data =
      std::get<std::vector<CMeta>>(*(m_pool->getData()))[entity_id];

  return meta_data.m_component_flags;
}
