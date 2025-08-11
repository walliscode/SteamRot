/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the EntityManager class
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "EntityManager.h"
#include "FlatbuffersConfigurator.h"
#include "PathProvider.h"
#include "emp_helpers.h"
#include <expected>
#include <variant>

namespace steamrot {

////////////////////////////////////////////////////////////
EntityManager::EntityManager() : m_archetype_manager(m_entity_memory_pool) {}

/////////////////////////////////////////////////
EntityManager::EntityManager(const size_t pool_size)
    : m_archetype_manager(m_entity_memory_pool) {
  // resize the entity memory pool to the given size
  ResizeEntityMemoryPool(pool_size);
}
/////////////////////////////////////////////////
EntityMemoryPool &EntityManager::GetEntityMemoryPool() {
  // return a reference to the memory pool
  return m_entity_memory_pool;
}

const ArchetypeManager &EntityManager::GetArchetypeManager() {
  // return a const reference to the archetype manager
  return m_archetype_manager;
}

////////////////////////////////////////////////////////////
void EntityManager::ResizeEntityMemoryPool(const size_t pool_size) {

  // use std::apply to resize the memory pool with lambda function
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      m_entity_memory_pool);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
EntityManager::ConfigureEntitiesFromDefaultData(const SceneType scene_type,
                                                const EnvironmentType env_type,
                                                const DataType data_type) {

  std::variant<FlatbuffersConfigurator> configurator(env_type);
  switch (data_type) {
  case DataType::Flatbuffers:
    configurator = FlatbuffersConfigurator{EnvironmentType::Test};
    break;
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue, "Invalid enum value"});
  }

  // call the configurator to configure entities
  auto result = std::visit(
      [&scene_type, this](auto &configurator_instance) {
        return configurator_instance.ConfigureEntitiesFromDefaultData(
            m_entity_memory_pool, scene_type);
      },
      configurator);
  return result;
}
////////////////////////////////////////////////////////////
size_t EntityManager::GetNextFreeEntityIndex() {

  // get CMeta vector from the pool
  std::vector<CMeta> meta_data =
      emp_helpers::GetComponentVector<CMeta>(m_entity_memory_pool);

  // find next inactive entity index
  for (size_t i = 0; i < meta_data.size(); ++i) {
    if (!meta_data[i].m_entity_active) {
      return i;
    }
  }
  // if no inactive entity found, return the size of the vector
  return meta_data.size();
};

} // namespace steamrot
